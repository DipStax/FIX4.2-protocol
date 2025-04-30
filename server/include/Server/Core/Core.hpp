#pragma once

#include "Server/Core/Pipeline/Router.hpp"
#include "Server/Core/Pipeline/DataRefresh.hpp"
#include "Server/Core/Pipeline/InNetwork.hpp"
#include "Server/Core/Pipeline/OutNetwork.hpp"
#include "Server/Core/Pipeline/UDPOutNetwork.hpp"
#include "Server/Core/MarketContainer.hpp"
#include "Server/Network/Processor.hpp"
#include "Server/Core/Pipeline/ProcessUnit.hpp"

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

        std::map<std::string, ProcessUnit<MarketContainer>> m_markets;

        InUDP m_q_udp;
        InputRouter m_q_router;
        MarketEntry m_q_markets;
        InOutNetwork m_q_tcp;

        ProcessUnit<pip::InNetwork<net::tcp::in::Basic>> m_tcp_input;
        ProcessUnit<pip::Router> m_router;
        ProcessUnit<pip::OutNetwork<net::tcp::out::Response>> m_tcp_output;
        ProcessUnit<pip::UDPOutNetwork> m_udp_output;
};