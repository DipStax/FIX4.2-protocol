#pragma once

#include "Server/ProcessUnit/data/Global.hpp"
#include "Server/OrderBook.hpp"

#include "Shared/ProcessUnit/AInputProcess.hpp"
#include "Shared/Thread/QueueMutex.hpp"

namespace pu::market
{
    class Cancel : public AInputProcess<Context<data::ParsedMessage<fix42::msg::OrderCancelRequest>>>
    {
        public:
            Cancel(QueueMutex<ProcessId> &_mutex, OrderBook &_ob, StringOutputQueue &_output);
            virtual ~Cancel() = default;

        protected:
            void onInput(InputType _input) final;

        private:
            void aknowledgeCancel(const Order _order, InputType _input);
            void rejectOrderAlreadyProcess(const OrderId &_origin, const OrderId &_cancel);
            void rejectOrderInvalidQty(const OrderId &_origin, const OrderId &_cancel);

            QueueMutex<ProcessId> m_mutex;

            StringOutputQueue &m_tcp_output;

            OrderBook &m_ob;
    };
}