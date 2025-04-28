#pragma once

#include "Server/Core/Pipeline/Naming.hpp"
#include "Server/Core/Pipeline/IProcessUnit.hpp"

namespace pip
{
    /// @brief Pipeline running action depending on received message.
    class Router : public IProcessUnit
    {
        public:
            /// @brief Core pipeline type.
            /// @brief Construct the pipeline.
            /// @param _input Input data queue of the pipeline.
            /// @param _output Output data queue of the pipeline.
            /// @param _raw Raw message queue send to the pip::OutNetwork pipeline.
            Router(InputRouter &_input, InMarketData &_data, InOutNetwork &_raw);
            virtual ~Router() = default;

            void registerMarket(const std::string &_name, InMarket &_input);

        protected:
            /// @brief Core function of the pipeline determining it's behavior
            void runtime(std::stop_token _st);

        protected:
            bool treatLogon(RouterInput &_input);
            bool treatLogout(RouterInput &_input);
            bool treatNewOrderSingle(RouterInput &_input);
            bool treatOrderCancelRequest(RouterInput &_input);
            bool treatOrderCancelReplaceRequest(RouterInput &_input);
            bool treatUnknown(RouterInput &_input);
            bool treatHeartbeat(RouterInput &_input);
            bool treatMarketDataRequest(RouterInput &_input);

        private:
            MarketEntry m_market_input;      ///< Map of every market ouput data queue.

            InputRouter &m_input;       ///< Intput data queue.
            InMarketData &m_q_data;      ///< Map of every market ouput data queue.
            InOutNetwork &m_q_raw;           ///< Raw message queue.
    };
}