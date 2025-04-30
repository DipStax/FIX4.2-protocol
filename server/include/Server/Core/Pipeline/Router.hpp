#pragma once

#include "Server/Core/Pipeline/Naming.hpp"
#include "Server/Core/Pipeline/IProcessUnit.hpp"

namespace pip
{
    /// @brief Pipeline running action depending on received message.
    class Router : public IProcessUnit<Context<RouterInput>>
    {
        public:
            /// @brief Core pipeline type.
            /// @brief Construct the pipeline.
            /// @param _input Input data queue of the pipeline.
            /// @param _output Output data queue of the pipeline.
            /// @param _raw Raw message queue send to the pip::OutNetwork pipeline.
            Router(InOutNetwork &_raw);
            virtual ~Router() = default;

            void registerMarket(const std::string &_name, InMarket &_input);

            InputType &getInput();

        protected:
            /// @brief Core function of the pipeline determining it's behavior
            void runtime(std::stop_token _st);

        protected:
            bool treatLogon(Context<RouterInput> &_input);
            bool treatLogout(Context<RouterInput> &_input);
            bool treatNewOrderSingle(Context<RouterInput> &_input);
            bool treatOrderCancelRequest(Context<RouterInput> &_input);
            bool treatOrderCancelReplaceRequest(Context<RouterInput> &_input);
            bool treatUnknown(Context<RouterInput> &_input);
            bool treatHeartbeat(Context<RouterInput> &_input);
            bool treatMarketDataRequest(Context<RouterInput> &_input);

        private:
            MarketEntry m_market_input;      ///< Map of every market ouput data queue.

            InputRouter m_input;       ///< Intput data queue.
            InOutNetwork &m_tcp_output;           ///< Raw message queue.
    };
}