#include "Server/Core/MarketContainer.hpp"
#include "Server/Core/Pipeline/Naming.hpp"

MarketContainer::MarketContainer(const std::string &_name, InUDP &_udp, InOutNetwork &_tcp, std::vector<ClientSocket> &_clients)
    : m_name(_name), m_ob(m_name, m_q_event),
        m_market(m_name, m_ob, m_q_action, _tcp),
        m_obevent(m_name, m_q_event, _udp, _tcp),
        m_notify(m_name, m_ob, _clients)
{
}

MarketContainer::~MarketContainer()
{
    (void)stop();
}

bool MarketContainer::start()
{
    if (m_market.start()) {
        if (m_obevent.start()) {
            if (m_notify.start()) {
                return true;
            }
            (void)m_obevent.stop();
        }
        (void)m_market.stop();
    }
    return false;
}

MarketContainer::ThreadStatus MarketContainer::stop()
{
    return ThreadStatus(m_market.stop(), m_obevent.stop(), m_notify.stop());
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

bool MarketContainer::status(float _to)
{
    return m_market.status(_to) && m_obevent.status(_to);
}

const std::string &MarketContainer::getName() const
{
    return m_name;
}

InMarket &MarketContainer::getInput()
{
    return m_q_action;
}