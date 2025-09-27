#pragma once

#include "Server/ProcessUnit/data/Global.hpp"
#include "Server/OrderBook.hpp"

#include "Shared/ProcessUnit/AInputProcess.hpp"
#include "Shared/Thread/QueueMutex.hpp"
#include "Server/ProcessUnit/data/ProcessId.hpp"

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
            std::optional<Order> verifyOrderState(const InputType &_input);
            void aknowledgeCancel(InputType _input, const Order &_order);
            std::optional<Order> verifyOrderStateWithLock(const InputType &_input);

            QueueMutex<ProcessId> &m_mutex;

            StringOutputQueue &m_tcp_output;

            OrderBook &m_ob;
    };
}