#pragma once

#include "Server/Core/OrderBook.hpp"
#include "Server/Core/Pipeline/Market.hpp"
#include "Server/Core/Pipeline/Naming.hpp"
#include "Server/Core/Pipeline/OBEvent.hpp"
#include "Server/Core/Pipeline/Notification.hpp"
#include "Server/Core/Pipeline/ProcessUnit.hpp"

class MarketContainer : public IProcessUnit
{
    public:
        MarketContainer(const std::string &_name, InUDP &_udp, InOutNetwork &_tcp);
        virtual ~MarketContainer() = default;

        [[nodiscard]] fix::MarketDataSnapshotFullRefresh refresh(const OrderBook::Subscription &_sub);
        [[nodiscard]] fix::MarketDataIncrementalRefresh update(const OrderBook::Subscription &_sub);
        void cache_flush();

        [[nodiscard]] const std::string &getMarketName() const;
        [[nodiscard]] InMarket &getInput();

    protected:
        void runtime(std::stop_token _st);

    private:
        const std::string m_name;

        OrderBook::EventQueue m_q_event;
        InMarket m_q_action;

        OrderBook m_ob;

        ProcessUnit<pip::Market> m_market;
        ProcessUnit<pip::OBEvent> m_obevent;
        ProcessUnit<pip::Notification> m_notify;
};