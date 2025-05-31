#pragma once

#include "Client/ProcessUnit/Network/TcpInput.hpp"
#include "Client/ProcessUnit/User/HeartBeat.hpp"
#include "Client/ProcessUnit/Router.hpp"

#include "Common/Container/ProcessUnit.hpp"
#include "Common/Log/ILogger.hpp"
#include "Common/Network/Socket.hpp"

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

        ProcessUnit<pu::HeartBeatHandler> m_heartbeat;

        ProcessUnit<pu::Router> m_router;

        ProcessUnit<pu::TcpInputNetwork> m_tcp_input;

        QueueMessage m_tmp;
        // ProcessUnit<pu::Router> m_router;
        // ProcessUnit<pu::User> m_user_handler;

        std::unique_ptr<log::ILogger> Logger = nullptr;
};