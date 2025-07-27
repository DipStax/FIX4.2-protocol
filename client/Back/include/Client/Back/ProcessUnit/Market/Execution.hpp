#pragma once

#include "Client/Back/ProcessUnit/TransitName.hpp"

#include "Client/Common/IPC/MessageType.hpp"

#include "Shared/ProcessUnit/AInputProcess.hpp"

namespace pu
{
    class Execution :  public AInputProcess<TransitMessage>
    {
        public:
            Execution(QueueMessage &_tcp_output);
            virtual ~Execution() = default;

        protected:
            void onInput(InputType _input) final;

        private:
            void treatNewOrder(const InputType &_input);
            void sendNotificationNew(const InputType &_input, ipc::MessageType _msgtype);

            QueueMessage &m_tcp_output;
    };
}