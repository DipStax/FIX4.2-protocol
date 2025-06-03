#pragma once

#include "Common/Network/BaseSocket.hpp"
#include "Common/Network/Socket.hpp"

namespace net
{
    template<IsSocketDomain T>
    class Acceptor : public BaseSocket
    {
        public:
            using Client = std::shared_ptr<T>;

            Acceptor();
            ~Acceptor() = default;

            bool listen(uint32_t _port);

            Client accept();
    };
}

#include "Common/Network/Acceptor.inl"
