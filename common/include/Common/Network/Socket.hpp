#pragma once

#include <string>
#include <netinet/in.h>
#include <sys/un.h>

#include "Common/Network/BaseSocket.hpp"
#include "Common/Network/Ip.hpp"

template<class T>
concept IsSocketType = requires () {
    { T::Type } -> std::same_as<const uint32_t &>;
};

template<class T>
concept IsSocketDomain = IsBaseSocket<T> && requires () {
    { T::Domain } -> std::same_as<const uint32_t &>;
};

template<class T>
concept IsINetSocketDomain = IsSocketDomain<T> && T::Domain == AF_INET && requires (T _sock, const net::Ip &_ip1, uint32_t _ip2, uint32_t _port) {
    { _sock.connect(_ip1, _port) } -> std::same_as<bool>;
    { _sock.connect(_ip2, _port) } -> std::same_as<bool>;
};

template<class T>
concept IsUnixSocketDomain = IsSocketDomain<T> && T::Domain == AF_UNIX && requires (T _sock, const std::string &_path) {
    { _sock.connect(_path) } -> std::same_as<bool>;
    { _sock.unlink() } -> std::same_as<bool>;
};

template<class T>
concept IsSocket = IsSocketDomain<T>;

namespace net
{
    namespace dom
    {
        class INet : public BaseSocket
        {
            public:
                static constexpr const uint32_t Domain = AF_INET;

                ~INet() = default;

                bool connect(const Ip &_ip, uint32_t _port);
                bool connect(uint32_t _ip, uint32_t _port);

            protected:
                INet(int _type, int _proto);
                INet(int _fd);

            private:
                struct sockaddr_in m_addr;
        };

        class Unix : public BaseSocket
        {
            public:
                static constexpr const uint32_t Domain = AF_UNIX;

                ~Unix() = default;

                bool connect(const std::string &_path);

                bool unlink();

            protected:
                Unix(int _type, int _proto);
                Unix(int _fd);

            private:
                sockaddr_un m_addr{};
        };
    }

    namespace type
    {
        template<IsSocketDomain T>
        class Stream : public T
        {
            public:
                static constexpr const uint32_t Type = SOCK_STREAM;

                Stream();
                Stream(int _fd);
                virtual ~Stream() = default;

                size_t send(const std::string &_data);
                size_t send(const uint8_t *_data, size_t _size);

                [[nodiscard]] std::string receive(size_t _size, int &_error);

            protected:
        };
    }

    using INetTcp = type::Stream<dom::INet>;
    using UnixTcp = type::Stream<dom::Unix >;
}


#include "Common/Network/Socket.inl"