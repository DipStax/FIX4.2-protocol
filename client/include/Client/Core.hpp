#pragma once

#include "Client/ProcessUnit/TransitName.hpp"
#include "Client/ProcessUnit/TcpInput.hpp"

#include "Common/Container/ProcessUnit.hpp"
#include "Common/Network/Socket.hpp"
#include "Common/Log/ILogger.hpp"

class Core
{
    public:
        Core(uint32_t _tcp_port, uint32_t _udp_port);
        ~Core();

        bool start();
        void stop();
    private:
        bool m_running;

        std::shared_ptr<net::tcp::Socket> m_server;

        ProcessUnit<pu::TcpInputNetwork> m_tcp_input;

        QueueTransit m_tmp;
        // ProcessUnit<pu::Router> m_router;
        // ProcessUnit<pu::User> m_user_handler;

        std::unique_ptr<log::ILogger> Logger = nullptr;

};