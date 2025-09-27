#pragma once

#include "Server/ProcessUnit/data/Global.hpp"
#include "Server/ProcessUnit/data/ProcessId.hpp"
#include "Server/OrderBook.hpp"

#include "Shared/ProcessUnit/AInputProcess.hpp"
#include "Shared/Thread/QueueMutex.hpp"

namespace pu::market
{
    class NewOrder : public AInputProcess<Context<data::ParsedMessage<fix42::msg::NewOrderSingle>>>
    {
        public:
            NewOrder(QueueMutex<ProcessId> &_mutex, OrderBook &_ob, StringOutputQueue &_output);
            virtual ~NewOrder() = default;

        protected:
            void onInput(InputType _input) final;

        private:
            void notSupportedOrderType(const InputType &_input);

            void newOrderLimit(const InputType &_input);
            void notSupportedSide(const InputType &_input);

            QueueMutex<ProcessId> &m_mutex;

            OrderBook &m_ob;

            StringOutputQueue &m_tcp_output;

            const std::string m_symbol;
    };
}