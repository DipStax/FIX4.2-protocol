#include "Server/Core/ProcessUnit/MarketContainer.hpp"

namespace pu
{
    MarketContainer::MarketContainer(const std::string &_symbol, InputNetworkOutput &_tcp_output)
        : AProcessUnit<InputType>("Server/Market/" + _symbol + "/Container"),
        m_event(_symbol, _tcp_output),
        m_ob(_symbol, m_event.getInput()),
        m_market(m_ob, _tcp_output),
        m_router(m_ob.getSymbol(), _tcp_output, m_market.getInput())
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
        m_event.start();
        while (!_st.stop_requested()) {
            m_router.status();
            m_market.status();
            m_event.status();
        }
        m_router.stop();
        m_market.stop();
        m_event.stop();
    }
}