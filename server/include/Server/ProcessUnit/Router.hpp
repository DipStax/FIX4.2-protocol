#pragma once

#include "Shared/ProcessUnit/AInputProcess.hpp"
#include "Server/ProcessUnit/data/Global.hpp"

#include "Shared/Log/ILogger.hpp"

namespace pu
{
    /// @brief Pipeline running action depending on received message.
    class Router : public AInputProcess<Context<data::UnparsedMessage>>
    {
        public:
            Router(UnparsedMessageQueue &_logon, UnparsedMessageQueue &_logout, UnparsedMessageQueue &_heartbeat, StringOutputQueue &_error);
            virtual ~Router() = default;

            // void registerMarket(const std::string &_name, QueueInputType &_input);

        protected:
            void onInput(InputType _input) final;

            // bool redirectToMarket(const InputType &_input);

            /// @brief Function call when the input has an unknown message type
            /// @param _input Input with the message type error
            void treatUnknown(const InputType &_input);
            /// @brief Function call when the input try to access a protected route, but isn't logged in
            /// @param _input Input with the not logged in client
            void treatRequireLogin(const InputType &_input);
            /// @brief SessionReject message handler
            /// @param _input Input with the `SessionReject` message
            void treatReject(const InputType &_input);

        private:
            // std::map<std::string, QueueInputType &> m_market_input;

            UnparsedMessageQueue &m_logon_handler;
            UnparsedMessageQueue &m_logout_handler;
            UnparsedMessageQueue &m_heartbeat_handler;

            StringOutputQueue &m_error;
    };
}