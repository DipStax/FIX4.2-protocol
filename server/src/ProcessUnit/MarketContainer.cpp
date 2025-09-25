#include "Server/ProcessUnit/MarketContainer.hpp"

namespace pu
{
    MarketContainer::MarketContainer(const std::string &_symbol, StringOutputQueue &_tcp_output)
        : AProcessUnitBase("Server/Market/" + _symbol + "/Container"),
        m_event(_symbol, _tcp_output),
        m_ob(_symbol, m_event.getInput()),
        m_market_neworder(m_ob, _tcp_output),
        m_market_cancel(m_ob, _tcp_output)
    {
    }

    const std::string &MarketContainer::getMarketSymbol() const
    {
        return m_ob.getSymbol();
    }

    MarketContainer::MarketTupleQueue MarketContainer::getInput()
    {
        return std::forward_as_tuple(m_market_neworder.getInput(), m_market_cancel.getInput());
    }

    void MarketContainer::runtime(std::stop_token _st)
    {
        Logger->log<logger::Level::Info>("Launching process unit runtime");

        m_market_neworder.start();
        m_market_cancel.start();
        m_event.start();
        while (!_st.stop_requested()) {
            m_market_neworder.status();
            m_market_cancel.status();
            m_event.status();
        }
        m_market_neworder.stop();
        m_market_cancel.stop();
        m_event.stop();
    }
}