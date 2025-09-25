#include "Shared/Thread/QueueMutex.hpp"

template<IsEnum T>
void QueueMutex<T>::lock(const T &_id)
{
    std::unique_lock<std::mutex> lk(m_mutex);

    m_cv.wait(lk, [this, _id] {
        return !m_queue.empty() && m_queue.front() == _id && !m_lock;
    });
    m_lock = true;
}

template<IsEnum T>
void QueueMutex<T>::unlock()
{
    std::lock_guard<std::mutex> lk(m_mutex);

    m_lock = false;
    m_queue.pop();
    m_cv.notify_all();
}

template<IsEnum T>
T QueueMutex<T>::front() const
{
    std::lock_guard<std::mutex> lk(m_mutex);

    return m_queue.front();
}

template<IsEnum T>
void QueueMutex<T>::allow(const T &id)
{
    {
        std::lock_guard<std::mutex> lk(m_mutex);
        m_queue.push(id);
    }
    m_cv.notify_all();
}
