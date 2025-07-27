#pragma once

#include "Client/Common/IPC/Message/Message.hpp"
#include "Client/Common/IPC/MessageType.hpp"

#include "Shared/Network/Buffer.hpp"
#include "Shared/ProcessUnit/ProcessUnit.hpp"

namespace ipc
{
    struct Helper
    {
        static net::Buffer Status(PUStatus _status);
        static net::Buffer Logon(const msg::Logon &_logon);
        static net::Buffer OrderSingle(const msg::OrderSingle &_order);
        static net::Buffer ExecutionEvent(const msg::Execution &_exec, MessageType _type);
    };
}