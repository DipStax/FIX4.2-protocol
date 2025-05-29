#pragma once

#include "Client/ProcessUnit/TransitName.hpp"

#include "Common/Container/IProcessUnit.hpp"
#include "Common/Log/ILogger.hpp"

namespace pu
{
    class Router : public IProcessUnit<TransitMessage>
    {
        public:
            Router(QueueTransit &_heartbeat);

            QueueTransit &getInput() override;

        protected:
            void runtime(std::stop_token _st) override;

        private:
            bool unknownMessage(InputType &_input);

            QueueTransit m_input;

            QueueTransit &m_heartbeat;

            std::unique_ptr<log::ILogger> Logger;
    };
}