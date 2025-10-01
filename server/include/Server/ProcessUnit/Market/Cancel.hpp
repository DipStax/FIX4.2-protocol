#pragma once

#include <thread>

#include "Server/ProcessUnit/data/Global.hpp"
#include "Server/OrderBook.hpp"
#include "Server/ProcessUnit/data/ProcessId.hpp"

#include "Shared/ProcessUnit/AInputProcess.hpp"
#include "Shared/ProcessUnit/IProcessUnitStopable.hpp"
#include "Shared/Thread/QueueMutex.hpp"

namespace pu::market
{
    class Cancel : public AInputProcess<Context<data::ParsedMessage<fix42::msg::OrderCancelRequest>>>, IProcessUnitStopable
    {
        public:
            Cancel(QueueMutex<ExecId> &_mutex, OrderBook &_ob, StringOutputQueue &_output);
            virtual ~Cancel() = default;

        protected:
            void setup() final;

            void onInput(InputType _input) final;

            void onStop() final;

            void orderProcessing(std::stop_token _st);

        private:
            std::optional<Order> verifyOrderState(const InputType &_input);
            std::optional<Order> verifyOrderStateWithLock(const InputType &_input);

            QueueMutex<ExecId> &m_mutex;

            StringOutputQueue &m_tcp_output;

            ts::Queue<InputType> m_internal_queue;

            OrderBook &m_ob;

            std::jthread m_thread{};
    };
}