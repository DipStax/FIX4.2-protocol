#include "Common/Thread/Queue.hpp"

namespace ts
{
    template<class T>
    bool Queue<T>::empty() const
    {
        return m_queue.empty();
    }

    template<class T>
    size_t Queue<T>::size() const
    {
        return m_queue.size();
    }

    template<class T>
    void Queue<T>::push(const T &_data)
    {
        std::lock_guard<std::mutex> guard(m_mutex);

        m_queue.push(_data);
    }

    template<class T>
    void Queue<T>::push(T &&_data)
    {
        std::lock_guard<std::mutex> guard(m_mutex);

        m_queue.push(std::move(_data));
    }

    template<class T>
    template<class ...Ts>
    void Queue<T>::append(Ts &&..._args)
    {
        std::lock_guard<std::mutex> guard(m_mutex);

        m_queue.emplace(std::forward<Ts>(_args)...);
    }

    template<class T>
    const T &Queue<T>::front() const
    {
        return m_queue.front();
    }

    template<class T>
    T &Queue<T>::front()
    {
        return m_queue.front();
    }

    template<class T>
    void Queue<T>::pop()
    {
        std::lock_guard<std::mutex> guard(m_mutex);

        m_queue.pop();
    }

    template<class T>
    T Queue<T>::pop_front()
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        T data = std::move(m_queue.front());

        m_queue.pop();
        return data;
    }
}