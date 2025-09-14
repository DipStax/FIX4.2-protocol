#pragma once

#include "Server/ProcessUnit/Market/OBAction.hpp"
#include "Server/ProcessUnit/Market/OBEvent.hpp"

#include "Shared/ProcessUnit/ProcessUnit.hpp"
#include "Shared/ProcessUnit/AProcessUnit.hpp"

namespace pu
{

    class MarketContainer : public AProcessUnit<Context<data::UnparsedMessage>>
    {
        public:
            MarketContainer(const std::string &_symbol, StringOutputQueue &_tcp_output);
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
    };
}