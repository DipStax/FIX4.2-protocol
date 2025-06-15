#pragma once

#include "Common/Container/IProcessUnit.hpp"
#include "Server/Core/ProcessUnit/data/Market.hpp"

#include "Common/Message/Message.hpp"
#include "Common/Log/ILogger.hpp"

namespace pu::market
{
    class Router : public IProcessUnit<Context<data::MarketRouterInput>>
    {
        public:
            Router(std::string _symbol, InputNetworkOutput &_tcp_output, InputOBAction &_ob_action);
            virtual ~Router() = default;

            QueueInputType &getInput();

        protected:
            void runtime(std::stop_token _st);

        private:
            bool treatNewOrderSingle(InputType &&_input);
            bool treatOrderCancelRequest(InputType &&_input);
            bool treatOrderCancelReplaceRequest(InputType &&_input);

            const std::string m_symbol;

            InputNetworkOutput &m_tcp_output;

            QueueInputType m_input;
            InputOBAction &m_ob_action;

            std::unique_ptr<logger::ILogger> Logger = nullptr;
    };
}