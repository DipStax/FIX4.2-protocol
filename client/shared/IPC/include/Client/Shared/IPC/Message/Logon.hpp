#pragma once

#include "Shared/Network/Buffer.hpp"

namespace ipc::msg
{
    struct Logon
    {
        std::string UserId;
        uint32_t SeqNum;
        float HeartBeat;

        friend net::Buffer &operator<<(net::Buffer &_buffer, const Logon &_logon);
        friend net::Buffer &operator>>(net::Buffer &_buffer, Logon &_logon);

        friend std::ostream &operator<<(std::ostream &_os, const Logon &_logon);
    };
}