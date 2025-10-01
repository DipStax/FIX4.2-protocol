#include "Shared/Thread/QueueMutex.hpp"
#include "Shared/Log/Manager.hpp"

template<class T>
void QueueMutex<T>::lock(const T &_id)
{
    while (empty() || front() != _id || m_lock) {}
    m_lock = true;
}

template<class T>
void QueueMutex<T>::unlock()
{
    std::unique_lock lock(m_mutex);

    m_lock = false;
    m_queue.pop_front();
}

template<class T>
bool QueueMutex<T>::empty() const
{
    std::shared_lock lock(m_mutex);

    return m_queue.empty();
}

template<class T>
const T &QueueMutex<T>::front() const
{
    std::shared_lock lock(m_mutex);

    return m_queue.front();
}

template<class T>
void QueueMutex<T>::allow(const T &_id)
{
    std::unique_lock lock(m_mutex);

    m_queue.push_back(_id);
}

template<class T>
bool QueueMutex<T>::finish(const T &_id)
{
    std::unique_lock lock(m_mutex);
    auto it = std::find(m_queue.rbegin(), m_queue.rend(), _id);

    if (it == m_queue.rend())
        return false;
    m_queue.erase(std::next(it).base());
    return true;
}
