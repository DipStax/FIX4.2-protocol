#pragma once

#include "Shared/ProcessUnit/AInputProcess.hpp"
#include "Server/Core/ProcessUnit/data/Market.hpp"

#include "Shared/Thread/Pool.hpp"
#include "Shared/Log/ILogger.hpp"

namespace pu::market
{
    /// @brief Pipeline managing the OrderBook.
    class OBAction : public AInputProcess<Context<data::OBActionInput>>
    {
        public:
            /// @brief Construct the pipeline.
            /// @param _ob OrderBook reference.
            /// @param _input Input data queue.
            /// @param _output Output data queue.
            OBAction(OrderBook &_ob, InputNetworkOutput &_output);
            virtual ~OBAction() = default;

        protected:
            void onInput(InputType _input) final;

        private:

            bool treatNewOrderSingle(InputType &_input);
            // void process(InputType &_data);

            // bool runAdd(const InputType &_data);
            // bool runModify(const InputType &_data);
            // bool runCancel(const InputType &_data);

            void rejectOrderIdExist(InputType &_input, const obs::OrderInfo &_order);
            void acknowledgeOrder(InputType &_input, const obs::OrderInfo &_order);

            InputNetworkOutput &m_tcp_output;

            OrderBook &m_ob;

            const std::string m_symbol;

            ThreadPool<1> m_tp{};
    };
}