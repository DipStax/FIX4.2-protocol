#pragma once

#include "Server/Core/Pipeline/Action.hpp"
#include "Server/Core/Pipeline/DataRefresh.hpp"
#include "Server/Core/Pipeline/InNetwork.hpp"
#include "Server/Core/Pipeline/OutNetwork.hpp"
#include "Server/Core/Pipeline/UDPOutNetwork.hpp"
#include "Server/Core/MarketContainer.hpp"
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

        std::vector<ClientSocket> m_clients;
        std::map<std::string, MarketContainer> m_markets;

        InUDP m_q_udp;
        InAction m_q_action;
        MarketEntry m_q_markets;
        InMarketData m_q_data;
        InOutNetwork m_q_repdata;
        InOutNetwork m_q_tcp;

        pip::InNetwork<net::tcp::Socket, net::tcp::in::Basic, ClientSocket> m_innet;
        pip::Action m_action;
        pip::DataRefresh m_data;

        pip::OutNetwork<net::tcp::Socket, net::tcp::out::Response, InOutNetwork, ClientSocket> m_outnet;
        pip::OutNetwork<net::tcp::Socket, net::tcp::out::SubResponse, InOutNetwork, ClientSocket> m_outdata;

        pip::UDPOutNetwork m_udp;
};