#pragma once

#include <string>
#include <sys/socket.h>
#include "Common/Network/BaseSocket.hpp"
#include "Common/Network/Ip.hpp"

namespace net
{
    namespace dom
    {
        template<IsSocketType T>
        class INet : public T
        {
            public:
                static constexpr const uint32_t Domain = AF_INET;

                INet();
                INet(int _fd);
                ~INet() = default;

                bool connect(const Ip &_ip, uint32_t _port);
                bool connect(uint32_t _ip, uint32_t _port);
        };

        template<IsSocketType T>
        class Unix : public T
        {
            public:
                static constexpr const uint32_t Domain = AF_UNIX;

                Unix();
                Unix(int _fd);
                ~Unix() = default;

                bool connect(const std::string &_path);
        };
    }

    namespace type
    {
        class Tcp : public BaseSocket
        {
            public:
                static constexpr const uint32_t Type = SOCK_STREAM;

                virtual ~Tcp() = default;

                size_t send(const std::string &_data);
                size_t send(const uint8_t *_data, size_t _size);

                [[nodiscard]] std::string receive(size_t _size, int &_error);

            protected:
                Tcp(int _type, bool _is_dom);
                Tcp(int _fd);
        };
    }

    using StreamTcp = dom::INet<type::Tcp>;
    using UnixTcp = dom::Unix<type::Tcp>;
}

template<class T>
concept IsSocketDomain = IsSocketType<T> && requires () {
    { T::Domain } -> std::same_as<const uint32_t &>;
    { T::Type } -> std::same_as<const uint32_t &>;
};

#include "Common/Network/Socket.inl"