#pragma once

#include "Client/Back/ProcessUnit/TransitName.hpp"

#include "Common/Container/IProcessUnit.hpp"
#include "Common/Log/ILogger.hpp"

namespace pu
{
    class Router : public IProcessUnit<TransitMessage>
    {
        public:
            Router(QueueMessage &_tcp_output, QueueTransit &_heartbeat, QueueTransit &_auth);

            QueueTransit &getInput() override;

        protected:
            void runtime(std::stop_token _st) override;

        private:
            bool unknownMessage(InputType &_input);
            bool treatReject(InputType &_input);

            QueueTransit m_input;

            QueueMessage &m_tcp_output;
            QueueTransit &m_heartbeat;
            QueueTransit &m_auth;

            std::unique_ptr<logger::ILogger> Logger;
    };
}