#pragma once

#include <cstdint>
#include <type_traits>

namespace net
{
    class BaseSocket
    {
        public:
            virtual ~BaseSocket();

            bool setBlocking(bool _flag);
            bool isBlocking() const;

            bool isOpen() const;
            operator bool() const;

            [[nodiscard]] uint32_t getPort() const;

            bool close();

            [[nodiscard]] int FD() const;

        protected:
            BaseSocket(int _dom, int _type, int _proto);
            BaseSocket(int _fd);

            bool create();

            int m_fd = -1;

        private:
            int m_dom = -1;
            int m_type = -1;
            int m_proto = -1;
    };
}

template<class T>
concept IsSocketType = std::is_base_of_v<net::BaseSocket, T>;
