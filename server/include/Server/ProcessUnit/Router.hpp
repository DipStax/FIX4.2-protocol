#pragma once

#include "Shared/ProcessUnit/AInputProcess.hpp"
#include "Server/ProcessUnit/data/Market.hpp"

#include "Shared/Log/ILogger.hpp"

namespace pu
{
    /// @brief Pipeline running action depending on received message.
    class Router : public AInputProcess<Context<data::RouterInput>>
    {
        public:
            Router(InputNetworkOutput &_raw, QueueInputType &_logon, QueueInputType &_logout, QueueInputType &_heartbeat);
            virtual ~Router() = default;

            void registerMarket(const std::string &_name, QueueInputType &_input);

        protected:
            void onInput(InputType _input) final;

        protected:
            bool redirectToMarket(const InputType &_input);

            bool treatUnknown(const InputType &_input);
            bool treatRequireLogin(const InputType &_input);
            bool treatReject(const InputType &_input);

        private:
            std::map<std::string, QueueInputType &> m_market_input;

            InputNetworkOutput &m_tcp_output;

            QueueInputType &m_logon_handler;
            QueueInputType &m_logout_handler;
            QueueInputType &m_heartbeat_handler;
    };
}