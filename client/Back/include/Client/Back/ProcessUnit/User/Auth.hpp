#pragma once

#include "Client/Back/ProcessUnit/TransitName.hpp"

#include "Common/Container/IProcessUnit.hpp"
#include "Common/Container/IProcessUnitStopable.hpp"
#include "Common/Log/ILogger.hpp"

namespace pu
{
    class AuthHandler : public IProcessUnit<TransitMessage>
    {
        public:
            AuthHandler(QueueMessage &_tcp_output);
            virtual ~AuthHandler() = default;

            QueueInputType &getInput() final;

        protected:
            void runtime(std::stop_token _st) final;

        private:
            bool handleLogon(InputType &_input);
            bool handleLogout(InputType &_input);

            QueueInputType m_input;

            QueueMessage &m_tcp_output;

            std::unique_ptr<log::ILogger> Logger = nullptr;
    };
}