#include "Server/Core/ProcessUnit/ProcessUnit.hpp"

template<class T>
template<class ...Ts>
ProcessUnit<T>::ProcessUnit(const std::string &_name, Ts &&..._args)
    : T(std::forward<Ts>(_args)...),
    m_status(PUStatus::Initialize), m_name(_name),
    m_future(m_promise.get_future())
{
}

template<class T>
void ProcessUnit<T>::start()
{
    m_thread = std::jthread(std::bind_front(&ProcessUnit<T>::process, this));
}

template<class T>
PUStatus ProcessUnit<T>::status(float _to)
{
    switch (m_future.wait_for(std::chrono::duration<float>(_to)))
    {
        case std::future_status::ready:
            m_future.get();
            return PUStatus::Stop;
        case std::future_status::timeout:
            return PUStatus::Running;
        case std::future_status::deferred:
            return PUStatus::Initialize;
        default:
            return PUStatus::Unknown;
    }
}

template<class T>
void ProcessUnit<T>::stop()
{
    if (m_thread.joinable()) {
        m_thread.request_stop();
        m_thread.join();
    }
    if (m_future.valid())
        m_future.wait();
}

template<class T>
const std::string &ProcessUnit<T>::getName() const
{
    return m_name;
}

template<class T>
void ProcessUnit<T>::process(std::stop_token _st)
{
    Logger::SetThreadName(THIS_THREAD_ID, getName());
    try {
        T::runtime(_st);
    } catch (...) {
        m_promise.set_exception(std::current_exception());
    }
}
