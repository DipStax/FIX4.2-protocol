#pragma once

#include "Client/Shared/IPC/Message/Message.hpp"
#include "Client/Shared/IPC/MessageType.hpp"

#include "Shared/Network/Buffer.hpp"
#include "Shared/ProcessUnit/ProcessUnit.hpp"

namespace ipc
{
    struct Helper
    {
        static net::Buffer FrontIdentification(const msg::IdentifyFront &_identify);
        static net::Buffer Status(PUStatus _status);
        static net::Buffer Logon(const msg::Logon &_logon);
        static net::Buffer OrderSingle(const msg::OrderSingle &_order);
        static net::Buffer ExecutionEvent(const msg::Execution &_exec, MessageType _type);
    };
}