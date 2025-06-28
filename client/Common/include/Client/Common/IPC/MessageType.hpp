#pragma once

#include "Common/Network/Buffer.hpp"

namespace ipc
{
    enum class MessageType : uint8_t
    {
        Status,
        Logon,
        OrderSingle,
        ExecNew,
        ExecEvent,
    };

    net::Buffer &operator<<(net::Buffer &_buffer, const MessageType &_type);
    net::Buffer &operator>>(net::Buffer &_buffer, MessageType &_type);
}