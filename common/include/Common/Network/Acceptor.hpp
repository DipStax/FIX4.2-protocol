#pragma once

#include "Common/Network/Socket.hpp"

namespace net
{
    template<IsSocket T>
    class Acceptor : public c::Socket
    {
        public:
            using Client = std::shared_ptr<T>;

            Acceptor();
            ~Acceptor();

            void blocking(bool _block);

            bool listen(uint32_t _port);

            Client accept();

            [[nodiscard]] bool close();
    };
}

#include "Common/Network/Acceptor.inl"
