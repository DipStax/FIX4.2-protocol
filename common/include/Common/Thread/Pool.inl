#include "Common/Thread/Pool.hpp"

template<size_t S>
ThreadPool<S>::ThreadPool()
{
    start();
}

template<size_t S>
ThreadPool<S>::~ThreadPool()
{
    stop();
}

template<size_t S>
void ThreadPool<S>::start()
{
    if (m_isStarted)
        return;
    for (size_t it = 0; it < S; it++)
        m_thread[it] = std::thread(&ThreadPool<S>::loop, this);
    m_isStarted = true;
}

template<size_t S>
void ThreadPool<S>::stop()
{
    if (!m_isStarted)
        return;
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        m_terminate = true;
    }
    m_cond.notify_all();
    for (size_t it = 0; it < S; it++)
        m_thread[it].join();
    m_isStarted = false;
}

template<size_t S>
void ThreadPool<S>::enqueue(Task _task)
{
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        m_queue.push(_task);
    }
    m_cond.notify_one();
}

template<size_t S>
bool ThreadPool<S>::empty()
{
    return m_queue.empty();
}

template<size_t S>
void ThreadPool<S>::loop()
{
    Task task;

    while (!m_terminate) {
        {
            std::unique_lock<std::mutex> lock(m_mutex);

            m_cond.wait(lock, [this] () {
                return !empty() || m_terminate;
            });
            if (m_terminate)
                    return;
            task = m_queue.pop_front();
        }
        task();
    }
}