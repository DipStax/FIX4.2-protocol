#pragma once

#include "Server/ProcessUnit/data/Global.hpp"
#include "Server/OrderBook.hpp"

#include "Shared/ProcessUnit/AInputProcess.hpp"
#include "Shared/Log/ILogger.hpp"
#include "Shared/Thread/Pool.hpp"

namespace pu::market
{
    class NewOrder : public AInputProcess<Context<data::ParsedMessage<fix42::msg::NewOrderSingle>>>
    {
        public:
            NewOrder(OrderBook &_ob, StringOutputQueue &_output);
            virtual ~NewOrder() = default;

        protected:
            void onInput(InputType _input) final;

        private:
            void notSupportedOrderType(const InputType &_input);

            void newOrderLimit(const InputType &_input);
            void notSupportedSide(const InputType &_input);

            StringOutputQueue &m_tcp_output;

            OrderBook &m_ob;

            const std::string m_symbol;
    };
}