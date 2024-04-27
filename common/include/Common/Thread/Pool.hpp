#pragma once

#include <array>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>

#include "Common/Thread/Queue.hpp"

#ifndef N_THREAD_TP
    #define N_THREAD_TP 1
#endif

template<size_t S = N_THREAD_TP>
class ThreadPool
{
    public:
        using Task = std::function<void()>;

        ThreadPool();
        ~ThreadPool();

        void start();
        void stop();

        void enqueue(Task _task);

        [[nodiscard]] bool empty();

    private:
        bool m_isStarted = false;
        bool m_terminate = false;

        ts::Queue<Task> m_queue{};

        std::array<std::thread, S> m_thread;
        std::mutex m_mutex;
        std::condition_variable m_cond;

        void loop();
};

#include "Common/Thread/Pool.inl"