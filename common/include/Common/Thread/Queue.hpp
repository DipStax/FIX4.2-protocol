#pragma once

#include <queue>
#include <mutex>

namespace ts
{
    template<class T>
    class Queue
    {
        public:
            Queue() = default;
            ~Queue() = default;

            using type = T;

            /// @brief Check if the queue is empty.
            /// @return True if the queue is empty otherwise false.
            [[nodiscard]] bool empty() const;
            /// @brief Get the size of the queue.
            /// @return the size of the queue.
            [[nodiscard]] size_t size() const;

            /// @brief Push an element to the end of the queue.
            /// @param _data Element to be pushed.
            void push(const T &_data);
            /// @brief Move an element to the end of the queue.
            /// @param _data Element to be moved.
            void push(T &&_data);

            /// @brief Construct an element at the end of the queue.
            /// @tparam ...Ts Parameters type.
            /// @param ..._args Parameters used to build the element to be pushed.
            template<class ...Ts>
            void append(Ts &&..._args);

            /// @brief Get the element in front of the queue.
            /// @return The front element.
            [[nodiscard]] const T &front() const;
            /// @brief Get the element in front of the queue.
            /// @return The front element.
            [[nodiscard]] T &front();

            /// @brief Delete the element in front of the queue.
            void pop();
            /// @brief Delete the front element from the queue while returning it.
            /// @return The element in front of the queue.
            [[nodiscard]] T pop_front();

        private:
            std::mutex m_mutex;
            std::queue<T> m_queue;
    };
}

#include "Common/Thread/Queue.inl"