#pragma once

#include "Client/Common/IPC/Message/Logon.hpp"

#include "Common/Network/Buffer.hpp"
#include "Common/Container/ProcessUnit.hpp"

namespace ipc
{
    struct Helper
    {
        static net::Buffer Status(PUStatus _status);
        static net::Buffer Logon(const msg::Logon &_logon);
    };
}