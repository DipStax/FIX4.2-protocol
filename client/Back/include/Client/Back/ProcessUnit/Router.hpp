#pragma once

#include "Client/Back/ProcessUnit/TransitName.hpp"

#include "Common/Container/AInputProcess.hpp"
#include "Common/Log/ILogger.hpp"

namespace pu
{
    class Router : public AInputProcess<TransitMessage>
    {
        public:
            Router(QueueMessage &_tcp_output, QueueTransit &_heartbeat, QueueTransit &_auth, QueueTransit &_exec);
            virtual ~Router() = default;

        protected:
            void onInput(InputType _input) final;

        private:
            bool unknownMessage(const InputType &_input);

            bool treatReject(const InputType &_input);
            bool treatBusinessReject(InputType &_input);

            QueueMessage &m_tcp_output;
            QueueTransit &m_heartbeat;
            QueueTransit &m_auth;
            QueueTransit &m_execution;
    };
}