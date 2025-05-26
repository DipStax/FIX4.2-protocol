#pragma once

#include "Server/Core/ProcessUnit/User/Logon.hpp"
#include "Server/Core/ProcessUnit/User/Logout.hpp"
#include "Server/Core/ProcessUnit/User/HeartBeat.hpp"

#include "Server/Core/ProcessUnit/Network/TCPInput.hpp"
#include "Server/Core/ProcessUnit/Network/TCPOutput.hpp"
#include "Server/Core/ProcessUnit/Network/UDPOutput.hpp"

#include "Server/Core/ProcessUnit/MarketContainer.hpp"

#include "Server/Core/ProcessUnit/Router.hpp"

#include "Server/Core/ProcessUnit/ProcessUnit.hpp"

#include "Common/Log/Manager.hpp"

class Core
{
    public:
        Core(uint32_t _tcp_port, uint32_t _udp_port);
        ~Core();

        [[nodiscard]] bool start();

        void stop();

    protected:
        bool internal_start();

        void market_init();

    private:

        bool m_running = false;

        std::map<std::string, ProcessUnit<pu::MarketContainer>> m_markets;

        ProcessUnit<pu::TcpOutputNetwork> m_tcp_output;
        ProcessUnit<pu::UdpOutputNetwork> m_udp_output;

        ProcessUnit<pu::user::LogonHandler> m_logon;
        ProcessUnit<pu::user::LogoutHandler> m_logout;
        ProcessUnit<pu::user::HeartBeatHandler> m_heartbeat;

        ProcessUnit<pu::Router> m_router;
        ProcessUnit<pu::TCPInputNetwork> m_tcp_input;

        std::unique_ptr<log::ILogger> Logger = nullptr;
};