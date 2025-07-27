#pragma once

#include <sys/epoll.h>

/// @brief C++ to C binding namespace.
namespace net::c
{
    /// @brief C++ binding class for file descriptor supporting epoll.
    class EPoll
    {

        public:
            /// @brief C++ using of a epoll event structure.
            using Event = struct epoll_event;

            EPoll() = default;
            EPoll(int _max);
            virtual ~EPoll() = default;

            /// @brief C++ binding to the C function epoll_create.
            /// @param _max Maximum number of socket supported at once
            /// @return 
            [[nodiscard]] static int create(int _max);
            /// @brief C++ binding to the C function epoll_wait.
            /// @param _fd Epoll file descriptor.
            /// @param _events Output of file descriptor event pending.
            /// @param _maxev Maximum event able to be store.
            /// @param _to Timeout of the waiting.
            /// @return 
            [[nodiscard]] static int wait(int _fd, Event *_events, int _maxev, int _to);
            /// @brief C++ binding to the C function epoll_ctl.
            /// @param _epfd 
            /// @param _op 
            /// @param _fd 
            /// @param _event 
            /// @return 
            [[nodiscard]] static int ctl(int _epfd, int _op, int _fd, Event *_event);

        protected:
            void create();

            /// @brief Wait for event.
            /// @param _events Event ouput.
            /// @param _to Timeout of the wait.
            /// @return 
            [[nodiscard]] int wait(Event *_events, int _to);
            /// @brief 
            /// @param _op 
            /// @param _fd 
            /// @param _event 
            /// @return 
            [[nodiscard]] int ctl(int _op, int _fd, Event *_event);

        private:
            int m_max = 0;  ///< Maximum number of socket supported at once by epoll.

            int m_fd = 0;   ///< File descriptor bind to epoll
    };
}