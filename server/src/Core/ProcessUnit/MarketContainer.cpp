#include "Server/Core/ProcessUnit/MarketContainer.hpp"

namespace pu
{
    MarketContainer::MarketContainer(const std::string &_name, InUDP &_udp, InOutNetwork &_tcp)
        : m_obevent(_name, _udp, _tcp),
        m_ob(_name, m_obevent.getInput()),
        m_market(m_ob, _tcp),
        m_notify(m_ob, _tcp),
        m_data_refresh(m_ob, _tcp)
    {
    }

    const std::string &MarketContainer::getMarketSymbol() const
    {
        return m_ob.getSymbol();
    }

    MarketContainer::QueueInputType &MarketContainer::getInput()
    {
        return m_input;
    }

    std::string MarketContainer::getThreadName() const
    {
        return std::string("Market Container - " + m_name);
    }

    void MarketContainer::runtime(std::stop_token _st)
    {
        Context<MarketInput> input;

        m_market.start();
        m_obevent.start();
        m_notify.start();
        while (!_st.stop_requested()) {
            // move to an other thread
            if (!m_input.empty()) {
                input = m_input.pop_front();

                switch (input.type)
                {
                    case MarketInput::Type::Order:
                        m_market.getInput().append(std::move(input));
                        break;
                    case MarketInput::Type::Refresh:
                        m_data_refresh.getInput().append(std::move(input));
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
}