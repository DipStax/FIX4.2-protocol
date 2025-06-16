#pragma once

#include "Common/Container/IProcessUnit.hpp"
#include "Server/Core/ProcessUnit/data/Market.hpp"

#include "Common/Thread/Pool.hpp"
#include "Common/Log/ILogger.hpp"

namespace pu::market
{
    /// @brief Pipeline managing the OrderBook.
    class OBAction : public IProcessUnit<Context<data::OBActionInput>>
    {
        public:
            /// @brief Construct the pipeline.
            /// @param _ob OrderBook reference.
            /// @param _input Input data queue.
            /// @param _output Output data queue.
            OBAction(OrderBook &_ob, InputNetworkOutput &_output);
            virtual ~OBAction() = default;

            [[nodiscard]] QueueInputType &getInput();

        protected:
            void runtime(std::stop_token _st);

        private:

            bool treatNewOrderSingle(InputType &_input);
            // void process(InputType &_data);

            // bool runAdd(const InputType &_data);
            // bool runModify(const InputType &_data);
            // bool runCancel(const InputType &_data);

            void rejectOrderIdExist(InputType &_input, const obs::OrderInfo &_order);
            void orderValidated(InputType &_input, const obs::OrderInfo &_order, const std::pair<OrderStatus, Quantity> &_result);


            QueueInputType m_input;
            InputNetworkOutput &m_tcp_output;

            OrderBook &m_ob;

            const std::string m_symbol;

            std::unique_ptr<logger::ILogger> Logger = nullptr;

            ThreadPool<1> m_tp{};
    };
}