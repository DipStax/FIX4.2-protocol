#pragma once

#include <memory>

#include "Client/Handler/Handler.hpp"
#include "Common/Network/Selector.hpp"
#include "Common/Network/Socket.hpp"
#include "Common/Network/UDPPackage.hpp"


namespace io
{
    class UDPHandler : public Handler<data::UDPPackage, data::UDPPackage>
    {
        public:
            using Client = net::Selector<net::udp::Socket>::Client;

            UDPHandler(const net::Ip &_ip, uint32_t _port);
            ~UDPHandler() = default;

            [[nodiscard]] uint32_t port() const;
            [[nodiscard]] net::Ip ip() const;

        protected:
            virtual void loop() override final;

        private:
            net::Ip m_ip;

            std::shared_ptr<net::udp::Socket> m_socket;
            net::Selector<net::udp::Socket> m_selector;
    };
}