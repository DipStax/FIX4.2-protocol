#pragma once

#include "Server/Core/ProcessUnit/Market/OBAction.hpp"
#include "Server/Core/ProcessUnit/Market/OBEvent.hpp"
#include "Server/Core/ProcessUnit/Market/Notification.hpp"
#include "Server/Core/ProcessUnit/Market/RefreshSuscribtion.hpp"
#include "Server/Core/ProcessUnit/Market/Router.hpp"
#include "Server/Core/ProcessUnit/ProcessUnit.hpp"

namespace pu
{

    class MarketContainer : public IProcessUnit<Context<data::MarketRouterInput>>
    {
        public:
            MarketContainer(const std::string &_symbol, InputUdp &_udp, InputNetworkOutput &_tcp_output);
            virtual ~MarketContainer() = default;

            [[nodiscard]] const std::string &getMarketSymbol() const;
            [[nodiscard]] QueueInputType &getInput();

        protected:
            std::string getThreadName() const;

            void runtime(std::stop_token _st);

        private:
            const std::string m_symbol;

            OrderBook::EventQueue m_q_event;
            QueueInputType m_input;

            ProcessUnit<pu::market::OBEvent> m_obevent;
            OrderBook m_ob;

            ProcessUnit<pu::market::OBAction> m_market;
            ProcessUnit<pu::market::Router> m_router;
            ProcessUnit<pu::market::Notification> m_notify;
            ProcessUnit<pu::market::RefreshSuscribtion> m_data_refresh;
    };
}