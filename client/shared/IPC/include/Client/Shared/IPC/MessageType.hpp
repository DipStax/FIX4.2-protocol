#pragma once

#include "Shared/Network/Buffer.hpp"

namespace ipc
{
    enum class MessageType : uint8_t
    {
        Identify,
        Status,
        Logon,
        OrderSingle,
        ExecNew,
        ExecEvent,
    };

    net::Buffer &operator<<(net::Buffer &_buffer, const MessageType &_type);
    net::Buffer &operator>>(net::Buffer &_buffer, MessageType &_type);
}