#pragma once

#include "Client/Handler/Handler.hpp"
#include "Common/Network/Selector.hpp"
#include "Common/Network/Socket.hpp"
#include "Common/Message/Serializer.hpp"
#include "Common/Message/Fix.hpp"

namespace io
{
    class TCPHandler : public Handler<fix::Message, fix::Serializer::AnonMessage>
    {
        public:
            TCPHandler(const net::Ip &_ip, uint32_t _port);
            ~TCPHandler() = default;

            using Client = std::shared_ptr<net::tcp::Socket>;

        protected:
            virtual void loop() override final;

        private:
            Client m_socket;
            net::Selector<net::tcp::Socket> m_selector;
    };
}