#pragma once

#include "Server/ProcessUnit/data/Market.hpp"

#include "Shared/ProcessUnit/AInputProcess.hpp"
#include "Shared/Message/Message.hpp"
#include "Shared/Log/ILogger.hpp"

namespace pu::market
{
    class Router : public AInputProcess<Context<data::MarketRouterInput>>
    {
        public:
            Router(std::string _symbol, InputNetworkOutput &_tcp_output, InputOBAction &_ob_action);
            virtual ~Router() = default;

        protected:
            void onInput(InputType _input) final;

        private:
            bool treatNewOrderSingle(InputType &&_input);
            bool treatOrderCancelRequest(InputType &&_input);
            bool treatOrderCancelReplaceRequest(InputType &&_input);

            const std::string m_symbol;

            InputNetworkOutput &m_tcp_output;

            InputOBAction &m_ob_action;
    };
}