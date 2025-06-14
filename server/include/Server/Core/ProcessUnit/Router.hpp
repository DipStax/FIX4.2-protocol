#pragma once

#include "Common/Container/IProcessUnit.hpp"
#include "Server/Core/ProcessUnit/data/Market.hpp"

#include "Common/Log/ILogger.hpp"

namespace pu
{
    /// @brief Pipeline running action depending on received message.
    class Router : public IProcessUnit<Context<data::RouterInput>>
    {
        public:
            Router(InputNetworkOutput &_raw, QueueInputType &_logon, QueueInputType &_logout, QueueInputType &_heartbeat);
            virtual ~Router() = default;

            void registerMarket(const std::string &_name, QueueInputType &_input);

            [[nodiscard]] QueueInputType &getInput();

        protected:
            std::string getThreadName() const;

            void runtime(std::stop_token _st);

        protected:
            void redirectToMarket(InputType &_input);

            bool treatUnknown(InputType &_input);
            bool treatRequireLogin(InputType &_input);
            bool treatReject(InputType &_input);

            bool treatMarketDataRequest(InputType &_input);

        private:
            std::map<std::string, QueueInputType &> m_market_input;

            QueueInputType m_input;

            InputNetworkOutput &m_tcp_output;

            QueueInputType &m_logon_handler;
            QueueInputType &m_logout_handler;
            QueueInputType &m_heartbeat_handler;

            std::unique_ptr<logger::ILogger> Logger = nullptr;
    };
}