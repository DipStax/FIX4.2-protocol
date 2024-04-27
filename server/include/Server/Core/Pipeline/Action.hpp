#pragma once

#include "Server/Core/Pipeline/Core.hpp"
#include "Server/Core/Pipeline/Naming.hpp"

namespace pip
{
    /// @brief Pipeline running action depending on received message.
    class Action : public Pipeline<Action>
    {
        public:
            /// @brief Core pipeline type.
            using PipeType = Pipeline<Action>;

            /// @brief Construct the pipeline.
            /// @param _input Input data queue of the pipeline.
            /// @param _output Output data queue of the pipeline.
            /// @param _raw Raw message queue send to the pip::OutNetwork pipeline.
            Action(MarketEntry &_markets, InAction &_input, InMarketData &_data, InOutNetwork &_raw);
            /// @brief Stop and then destroy the pipeline.
            ~Action();

            /// @brief Run the pipeline
            /// @return Return true if the pipeline as correctly started else false.
            [[nodiscard]] bool start();

            /// @brief Core function of the pipeline determining it's behavior
            void loop();

        protected:
            bool treatLogon(ActionInput &_input);
            bool treatLogout(ActionInput &_input);
            bool treatNewOrderSingle(ActionInput &_input);
            bool treatOrderCancelRequest(ActionInput &_input);
            bool treatOrderCancelReplaceRequest(ActionInput &_input);
            bool treatUnknown(ActionInput &_input);
            bool treatHeartbeat(ActionInput &_input);
            bool treatMarketDataRequest(ActionInput &_input);

        private:
            MarketEntry &m_markets;      ///< Map of every market ouput data queue.

            InAction &m_input;       ///< Intput data queue.
            InMarketData &m_q_data;      ///< Map of every market ouput data queue.
            InOutNetwork &m_q_raw;           ///< Raw message queue.
    };
}