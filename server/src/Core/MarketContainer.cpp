#include "Server/Core/MarketContainer.hpp"
#include "Server/Core/Pipeline/Naming.hpp"

MarketContainer::MarketContainer(const std::string &_name, InUDP &_udp, InOutNetwork &_tcp)
    : m_obevent("OBEvent-" + m_name, m_name, _udp, _tcp),
    m_ob(m_name, m_obevent.getInput()),
    m_market("Market-" + m_name, m_ob, m_q_order, _tcp),
    m_notify("Notif-" + m_name, m_ob, _tcp),
    m_data_refresh("Refresh-" + m_name, m_ob, m_q_refresh, _tcp);
{
}

const std::string &MarketContainer::getMarketSymbol() const
{
    return m_ob.getSymbol();
}

InMarket &MarketContainer::getInput()
{
    return m_q_input;
}

void MarketContainer::runtime(std::stop_token _st)
{
    Context<MarketInput> input;

    m_market.start();
    m_obevent.start();
    m_notify.start();
    while (!_st.stop_requested()) {
        // move to an other thread
        if (!m_q_action.empty()) {
            input = m_q_action.pop_front();

            switch (input.type)
            {
            case MarketInput::Type::Order:
                break;
            case MarketInput::Type::Refresh:
                
                break;
            default:
                break;
            }
        }

        m_market.status();
        m_obevent.status();
        m_notify.status();
    }
    m_market.stop();
    m_obevent.stop();
    m_notify.stop();
}