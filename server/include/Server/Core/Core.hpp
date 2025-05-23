#pragma once

#include "Server/Core/ProcessUnit/User/Logon.hpp"
#include "Server/Core/ProcessUnit/User/Logout.hpp"
#include "Server/Core/ProcessUnit/User/HeartBeat.hpp"
#include "Server/Core/ProcessUnit/InNetwork.hpp"
#include "Server/Core/ProcessUnit/MarketContainer.hpp"
#include "Server/Core/ProcessUnit/OutNetwork.hpp"
#include "Server/Core/ProcessUnit/ProcessUnit.hpp"
#include "Server/Core/ProcessUnit/Router.hpp"
#include "Server/Core/ProcessUnit/UDPOutNetwork.hpp"
#include "Server/Network/Processor.hpp"

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

        ProcessUnit<pu::OutNetwork<net::tcp::out::Response>> m_tcp_output;
        ProcessUnit<pu::UDPOutNetwork> m_udp_output;

        ProcessUnit<pu::user::LogonHandler> m_logon;
        ProcessUnit<pu::user::LogoutHandler> m_logout;
        ProcessUnit<pu::user::HeartBeatHandler> m_heartbeat;

        ProcessUnit<pu::Router> m_router;
        ProcessUnit<pu::InNetwork<net::tcp::in::Basic>> m_tcp_input;
};