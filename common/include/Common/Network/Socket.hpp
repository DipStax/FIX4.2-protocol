#pragma once

#include <string>
#include <vector>

#include <netinet/in.h>
#include <sys/un.h>

#include "Common/Network/BaseSocket.hpp"
#include "Common/Network/Ip.hpp"
#include "Common/Network/NetworkBuffer.hpp"

template<class T>
concept IsSocketType = requires (T _sock, const std::byte *_data1, size_t _size, std::string _data2, int &_err) {
    { T::Type } -> std::same_as<const uint32_t &>;
    { _sock.send(_data1, _size) } -> std::same_as<size_t>;
    { _sock.send(_data2) } -> std::same_as<size_t>;
    { _sock.receive(_size, _err) } -> std::same_as<std::vector<std::byte>>;
};

template<class T>
concept IsDGramSocketType = IsSocketType<T> && T::Type == SOCK_DGRAM;

template<class T>
concept IsStreamSocketType = IsSocketType<T> && T::Type == SOCK_STREAM;

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

                virtual ~INet() = default;

                bool connect(const Ip &_ip, uint32_t _port);
                bool connect(uint32_t _ip, uint32_t _port);

            protected:
                INet(int _type, int _proto);
                INet(int _fd);

                struct sockaddr_in m_addr;
        };

        class Unix : public BaseSocket
        {
            public:
                static constexpr const uint32_t Domain = AF_UNIX;

                virtual ~Unix() = default;

                bool connect(const std::string &_path);

                bool unlink();

            protected:
                Unix(int _type, int _proto);
                Unix(int _fd);

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
                ~Stream() = default;

                // size_t send(const NetworkBuffer &_data);
                size_t send(const std::string &_data);
                size_t send(const std::byte *_data, size_t _size);

                [[nodiscard]] std::vector<std::byte> receive(size_t _size, int &_error);
        };

        template<IsSocketDomain T>
        class DGram : public T
        {
            public:
                static constexpr const uint32_t Type = SOCK_DGRAM;

                DGram();
                DGram(int _fd);
                ~DGram() = default;

                // size_t send(const NetworkBuffer &_data);
                size_t send(const std::string &_data);
                size_t send(const std::byte *_data, size_t _size);

                [[nodiscard]] std::vector<std::byte> receive(size_t _size, int &_error);

                bool setBroadcast(bool _bc)
                    requires IsINetSocketDomain<T>;
                [[nodiscard]] bool isBroadcast() const
                    requires IsINetSocketDomain<T>;

            private:
                bool m_broadcast;
        };
    }

    using INetTcp = type::Stream<dom::INet>;
    using INetUdp = type::DGram<dom::INet>;
    using UnixStream = type::Stream<dom::Unix>;
    using UnixDGram = type::DGram<dom::Unix>;
}


#include "Common/Network/Socket.inl"