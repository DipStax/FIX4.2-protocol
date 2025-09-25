#pragma once

#include "Server/ProcessUnit/data/Global.hpp"
#include "Server/OrderBook.hpp"

#include "Shared/ProcessUnit/AInputProcess.hpp"

namespace pu::market
{
    class Cancel : public AInputProcess<Context<data::ParsedMessage<fix42::msg::OrderCancelRequest>>>
    {
        public:
            Cancel(OrderBook &_ob, StringOutputQueue &_output);
            virtual ~Cancel() = default;

        protected:
            void onInput(InputType _input) final;

        private:
            StringOutputQueue &m_tcp_output;

            OrderBook &m_ob;
    };
}