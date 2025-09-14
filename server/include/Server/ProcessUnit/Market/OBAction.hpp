#pragma once

#include "Server/ProcessUnit/data/Global.hpp"
#include "Server/OrderBook.hpp"

#include "Shared/ProcessUnit/AInputProcess.hpp"
#include "Shared/Log/ILogger.hpp"
#include "Shared/Thread/Pool.hpp"

namespace pu::market
{
    class OBAction : public AInputProcess<Context<data::UnparsedMessage>>
    {
        public:
            OBAction(OrderBook &_ob, StringOutputQueue &_output);
            virtual ~OBAction() = default;

        protected:
            void onInput(InputType _input) final;

        private:
            void treatNewOrderSingle(InputType &_input);

            void newOrderLimit(const InputType &_input, const fix42::msg::NewOrderSingle &_order);

            StringOutputQueue &m_tcp_output;

            OrderBook &m_ob;

            const std::string m_symbol;

            ThreadPool<1> m_tp{};
    };
}