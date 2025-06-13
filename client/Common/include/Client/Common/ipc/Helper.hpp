#pragma once

#include "Common/Network/Buffer.hpp"
#include "Common/Container/ProcessUnit.hpp"

namespace ipc
{
    struct Helper
    {
        static net::Buffer Status(PUStatus _status);
        static net::Buffer Logon(const std::string &_name, uint32_t _seqnum);
    };
}