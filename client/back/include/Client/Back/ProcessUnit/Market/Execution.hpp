#pragma once

#include "Client/Back/ProcessUnit/data/Global.hpp"

#include "Client/Shared/IPC/MessageType.hpp"

#include "Shared/ProcessUnit/AInputProcess.hpp"
#include "Shared/Message-v2/Message.hpp"

namespace pu
{
    class Execution :  public AInputProcess<Context<data::UnparsedMessage>>
    {
        public:
            Execution(StringOutputQueue &_tcp_output);
            virtual ~Execution() = default;

        protected:
            void onInput(InputType _input) final;

        private:
            void treatNewOrder(const InputType &_input, const fix42::msg::ExecutionReport &_report);

            void notifyExecEvent(const InputType &_input, const fix42::msg::ExecutionReport &_report);
            void notifyExecNew(const InputType &_input, const fix42::msg::ExecutionReport &_report);

            StringOutputQueue &m_tcp_output;
    };
}