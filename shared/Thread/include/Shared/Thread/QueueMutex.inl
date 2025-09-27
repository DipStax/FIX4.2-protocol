#include "Shared/Thread/QueueMutex.hpp"
#include "Shared/Log/Manager.hpp"

template<IsEnum T>
void QueueMutex<T>::lock(const T &_id)
{
    while (!m_queue.empty() && m_queue.front() == _id && !m_lock) {}
    m_lock = true;
}

template<IsEnum T>
void QueueMutex<T>::unlock()
{
    m_lock = false;
    m_queue.pop();
}

template<IsEnum T>
T QueueMutex<T>::front()
{
    return m_queue.front();
}

template<IsEnum T>
void QueueMutex<T>::allow(const T &_id)
{
    m_queue.push(_id);
}