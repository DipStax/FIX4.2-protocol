#pragma once

#include "Server/ProcessUnit/Market/OBAction.hpp"
#include "Server/ProcessUnit/Market/OBEvent.hpp"
#include "Server/ProcessUnit/Market/Notification.hpp"
#include "Server/ProcessUnit/Market/Router.hpp"

#include "Shared/ProcessUnit/ProcessUnit.hpp"
#include "Shared/ProcessUnit/AProcessUnit.hpp"

namespace pu
{

    class MarketContainer : public AProcessUnit<Context<data::MarketRouterInput>>
    {
        public:
            MarketContainer(const std::string &_symbol, InputNetworkOutput &_tcp_output);
            virtual ~MarketContainer() = default;

            [[nodiscard]] const std::string &getMarketSymbol() const;

            [[nodiscard]] QueueInputType &getInput();

        protected:
            void runtime(std::stop_token _st) final;

        private:
            const std::string m_symbol;

            ProcessUnit<pu::market::OBEvent> m_event;

            OrderBook m_ob;

            ProcessUnit<pu::market::OBAction> m_market;
            ProcessUnit<pu::market::Router> m_router;
    };
}