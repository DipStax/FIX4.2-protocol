#pragma once

#include "Server/ProcessUnit/data/Global.hpp"
#include "Server/ProcessUnit/data/ProcessId.hpp"
#include "Server/OrderBook.hpp"

#include "Shared/ProcessUnit/AInputProcess.hpp"
#include "Shared/ProcessUnit/IProcessUnitStopable.hpp"
#include "Shared/Thread/QueueMutex.hpp"

namespace pu::market
{
    class NewOrder : public AInputProcess<Context<data::ParsedMessage<fix42::msg::NewOrderSingle>>>, IProcessUnitStopable
    {
        public:
            NewOrder(QueueMutex<ExecId> &_mutex, OrderBook &_ob, StringOutputQueue &_output);
            virtual ~NewOrder() = default;

        protected:
            void setup() final;

            void onInput(InputType _input) final;

            void onStop() final;

            void orderProcessing(std::stop_token _st);

        private:
            void notSupportedOrderType(const InputType &_input);
            void notSupportedSide(const InputType &_input);

            QueueMutex<ExecId> &m_mutex;

            OrderBook &m_ob;

            std::jthread m_thread;
            ts::Queue<InputType> m_internal_queue;

            StringOutputQueue &m_tcp_output;

            const std::string m_symbol;
    };
}