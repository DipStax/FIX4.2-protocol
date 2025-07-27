#pragma once

#include "Server/Core/ProcessUnit/Market/OBAction.hpp"
#include "Server/Core/ProcessUnit/Market/OBEvent.hpp"
#include "Server/Core/ProcessUnit/Market/Notification.hpp"
#include "Server/Core/ProcessUnit/Market/Router.hpp"

#include "Common/Container/ProcessUnit.hpp"
#include "Common/Container/AProcessUnit.hpp"

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