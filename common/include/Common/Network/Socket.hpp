#pragma once

#include <string>

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