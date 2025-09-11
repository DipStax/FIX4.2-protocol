#pragma once

#include <array>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>

#include "Shared/Thread/Queue.hpp"

#ifndef N_THREAD_TP
    #define N_THREAD_TP 1
#endif


enum TPFinishStrategy {
    WaitEmpty,
    Direct
};

template<size_t S = N_THREAD_TP>
class ThreadPool
{
    public:
        static_assert(S > 0, "Number of thread is invalid");

        using Task = std::function<void()>;

        ThreadPool();
        ~ThreadPool();

        void start();
        void stop(TPFinishStrategy _finish = TPFinishStrategy::Direct);

        void enqueue(Task _task);

        [[nodiscard]] bool empty();

    private:
        bool m_isStarted = false;
        bool m_terminate = false;

        ts::Queue<Task> m_queue{};

        std::array<std::thread, S> m_thread;
        std::mutex m_mutex;
        std::condition_variable m_cond;

        TPFinishStrategy m_finish = TPFinishStrategy::Direct;

        void loop();
};

#include "Shared/Thread/Pool.inl"