#include <functional>

#include "Shared/ProcessUnit/ProcessUnit.hpp"

template<IsProcessUnitBase T>
template<class ...Ts>
ProcessUnit<T>::ProcessUnit(Ts &&..._args)
    : T(std::forward<Ts>(_args)...),
    m_future(m_promise.get_future())
{
}

template<IsProcessUnitBase T>
void ProcessUnit<T>::start()
{
    m_thread = std::jthread(&ProcessUnit<T>::process, this);
}

template<IsProcessUnitBase T>
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

template<IsProcessUnitBase T>
void ProcessUnit<T>::stop()
{
    if (m_thread.joinable()) {
        m_thread.request_stop();
        m_thread.join();
    }
    if (m_future.valid())
        m_future.wait();
}

template<IsProcessUnitBase T>
void ProcessUnit<T>::process(std::stop_token _st)
{
    try {
        this->Logger->template log<logger::Level::Info>("Starting process unit...");
        T::runtime(_st);
        m_promise.set_value();
    } catch (...) {
        this->Logger->template log<logger::Level::Fatal>("Process unit have crashed");
        m_promise.set_exception(std::current_exception());
    }

    if constexpr (IsPUStopable<T>) {
        this->Logger->template log<logger::Level::Debug>("Calling OnStop function for gracefull exit");
        this->onStop();
    }
    this->Logger->template log<logger::Level::Info>("Process unit exit");
}
