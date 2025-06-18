#include "Server/Core/ProcessUnit/MarketContainer.hpp"

namespace pu
{
    MarketContainer::MarketContainer(const std::string &_symbol, InputUdp &_udp, InputNetworkOutput &_tcp_output)
        : AProcessUnit<InputType>("Server/Market/" + _symbol + "/Container"),
        // m_obevent(_symbol, _udp, _tcp_output),
        m_ob(_symbol),
        m_market(m_ob, _tcp_output),
        m_router(m_ob.getSymbol(), _tcp_output, m_market.getInput())
        // m_notify(m_ob, _tcp_output),
        //m_data_refresh(m_ob, _tcp_output)
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

    void MarketContainer::runtime(std::stop_token _st)
    {
        Logger->log<logger::Level::Info>("Launching process unit runtime");

        m_router.start();
        m_market.start();
        // m_obevent.start();
        // m_notify.start();
        while (!_st.stop_requested()) {
            m_router.status();
            m_market.status();
            // m_obevent.status();
            // m_notify.status();
        }
        m_router.stop();
        m_market.stop();
        // m_obevent.stop();
        // m_notify.stop();
    }
}