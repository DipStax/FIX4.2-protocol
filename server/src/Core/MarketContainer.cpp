#include "Server/Core/MarketContainer.hpp"
#include "Server/Core/Pipeline/Naming.hpp"

MarketContainer::MarketContainer(const std::string &_name, InUDP &_udp, InOutNetwork &_tcp)
    : m_name(_name), m_ob(m_name, m_q_event),
        m_market("Market-" + m_name, m_ob, m_q_action, _tcp),
        m_obevent("OBEvent-" + m_name, m_name, m_q_event, _udp, _tcp),
        m_notify("Notif-" + m_name, m_name, m_ob)
{
}

void MarketContainer::runtime(std::stop_token _st)
{
    m_market.start();
    m_obevent.start();
    m_notify.start();
    while (!_st.stop_requested()) {
        m_market.status();
        m_obevent.status();
        m_notify.status();
    }
    m_market.stop();
    m_obevent.stop();
    m_notify.stop();
}

fix::MarketDataSnapshotFullRefresh MarketContainer::refresh(const OrderBook::Subscription &_sub)
{
    return m_ob.refresh(_sub);
}

fix::MarketDataIncrementalRefresh MarketContainer::update(const OrderBook::Subscription &_sub)
{
    return m_ob.update(_sub);
}

void MarketContainer::cache_flush()
{
    m_ob.cache_flush();
}

const std::string &MarketContainer::getMarketName() const
{
    return m_name;
}

InMarket &MarketContainer::getInput()
{
    return m_q_action;
}