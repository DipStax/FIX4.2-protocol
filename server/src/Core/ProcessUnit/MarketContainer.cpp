#include "Server/Core/ProcessUnit/MarketContainer.hpp"

namespace pu
{
    MarketContainer::MarketContainer(const std::string &_symbol, InputUdp &_udp, InputNetworkOutput &_tcp_output)
        : m_obevent(_symbol, _udp, _tcp_output),
        m_ob(_symbol, m_obevent.getInput()),
        m_market(m_ob, _tcp_output),
        m_router(m_ob.getSymbol(), _tcp_output, m_market.getInput()),
        m_notify(m_ob, _tcp_output),
        m_data_refresh(m_ob, _tcp_output)
    {
    }

    const std::string &MarketContainer::getMarketSymbol() const
    {
        return m_ob.getSymbol();
    }

    MarketContainer::QueueInputType &MarketContainer::getInput()
    {
        return m_router.getInput();
    }

    std::string MarketContainer::getThreadName() const
    {
        return "Market Container - " + m_symbol;
    }

    void MarketContainer::runtime(std::stop_token _st)
    {
        m_router.start();
        m_market.start();
        m_obevent.start();
        m_notify.start();
        while (!_st.stop_requested()) {
            m_router.status();
            m_market.status();
            m_obevent.status();
            m_notify.status();
        }
        m_router.stop();
        m_market.stop();
        m_obevent.stop();
        m_notify.stop();
    }
}