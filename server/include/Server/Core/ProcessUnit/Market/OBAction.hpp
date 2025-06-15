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
            void process(InputType &_data);

            bool runAdd(const InputType &_data);
            bool runModify(const InputType &_data);
            bool runCancel(const InputType &_data);

            const std::string m_name;

            QueueInputType m_input;
            InputNetworkOutput &m_tcp_output;

            ThreadPool<1> m_tp;

            OrderBook &m_ob;

            std::unique_ptr<logger::ILogger> Logger = nullptr;
    };
}