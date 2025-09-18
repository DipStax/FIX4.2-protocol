#pragma once

#include "Client/Back/ProcessUnit/data/Global.hpp"

#include "Shared/ProcessUnit/AInputProcess.hpp"
#include "Shared/Log/ILogger.hpp"

namespace pu
{
    class Router : public AInputProcess<Context<data::UnparsedMessage>>
    {
        public:
            Router(UnparsedMessageQueue &_heartbeat, UnparsedMessageQueue &_auth, UnparsedMessageQueue &_exec, StringOutputQueue &_tcp_output);
            virtual ~Router() = default;

        protected:
            void onInput(InputType _input) final;

        private:
            void unknownMessage(const InputType &_input);

            void treatReject(const InputType &_input);
            void treatBusinessReject(InputType &_input);

            UnparsedMessageQueue &m_heartbeat;
            UnparsedMessageQueue &m_auth;
            UnparsedMessageQueue &m_execution;

            StringOutputQueue &m_error;
    };
}