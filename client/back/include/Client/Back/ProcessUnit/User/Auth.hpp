#pragma once

#include "Client/Back/ProcessUnit/data/Global.hpp"

#include "Shared/ProcessUnit/AInputProcess.hpp"
#include "Shared/Log/ILogger.hpp"

namespace pu
{
    class AuthHandler : public AInputProcess<Context<data::UnparsedMessage>>
    {
        public:
            AuthHandler(StringOutputQueue &_tcp_output);
            virtual ~AuthHandler() = default;

        protected:
            void onInput(InputType _input) final;

        private:
            void handleLogon(InputType &_input);
            void handleLogout(InputType &_input);

            StringOutputQueue &m_tcp_output;
    };
}