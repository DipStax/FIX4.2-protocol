#pragma once

#include <deque>

template<class T>
class QueueMutex
{
    public:
        QueueMutex() = default;
        ~QueueMutex() = default;

        void lock(const T &_id);
        void unlock();

        [[nodiscard]] bool empty() const;
        [[nodiscard]] const T &front() const;

        void allow(const T &id);

        bool finish(const T &_id);

    private:
        mutable std::shared_mutex m_mutex;

        std::deque<T> m_queue{};
        bool m_lock = false;
};

#include "Shared/Thread/QueueMutex.inl"