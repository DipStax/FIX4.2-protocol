#pragma once

#include "Client/Back/ProcessUnit/TransitName.hpp"

#include "Common/Container/AInputProcess.hpp"
#include "Common/Container/IProcessUnitStopable.hpp"
#include "Common/Log/ILogger.hpp"

namespace pu
{
    class AuthHandler : public AInputProcess<TransitMessage>
    {
        public:
            AuthHandler(QueueMessage &_tcp_output);
            virtual ~AuthHandler() = default;

        protected:
            void onInput(InputType _input) final;

        private:
            bool handleLogon(InputType &_input);
            bool handleLogout(InputType &_input);

            QueueMessage &m_tcp_output;
    };
}