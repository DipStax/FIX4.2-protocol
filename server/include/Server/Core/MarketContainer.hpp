#pragma once

#include "Server/Core/OrderBook.hpp"
#include "Server/Core/Pipeline/Market.hpp"
#include "Server/Core/Pipeline/Naming.hpp"
#include "Server/Core/Pipeline/OBEvent.hpp"
#include "Server/Core/Pipeline/Notification.hpp"

class MarketContainer
{
    public:
        MarketContainer(const std::string &_name, InUDP &_udp, InOutNetwork &_tcp, std::vector<ClientSocket> &_clients);
        ~MarketContainer();

        using ThreadStatus = std::tuple<std::future_status, std::future_status, std::future_status>;

        [[nodiscard]] bool start();

        [[nodiscard]] ThreadStatus stop();

        [[nodiscard]] fix::MarketDataSnapshotFullRefresh refresh(const OrderBook::Subscription &_sub);
        [[nodiscard]] fix::MarketDataIncrementalRefresh update(const OrderBook::Subscription &_sub);
        void cache_flush();

        bool status(float _to = 1.f);

        [[nodiscard]] const std::string &getName() const;
        [[nodiscard]] InMarket &getInput();

    private:
        const std::string m_name;

        OrderBook::EventQueue m_q_event;
        InMarket m_q_action;

        OrderBook m_ob;

        pip::Market m_market;
        pip::OBEvent m_obevent;
        pip::Notification m_notify;
};