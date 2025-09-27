#pragma once

#include <concepts>

template<class T>
concept IsEnum = std::is_enum_v<T>;

template<IsEnum T>
class QueueMutex
{
    public:
        QueueMutex() = default;
        ~QueueMutex() = default;

        void lock(const T &_id);
        void unlock();

        [[nodiscard]] T front();

        void allow(const T &id);

    private:
        std::mutex m_mutex{};
        std::condition_variable m_cv{};
        std::queue<T> m_queue{};
        bool m_lock = false;
};

#include "Shared/Thread/QueueMutex.inl"