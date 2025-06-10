#pragma once

#include <cstdint>

#include "Common/Network/Buffer.hpp"

namespace ipc
{
    enum class MessageType : uint8_t
    {
        Status
    };

    struct Header
    {
        MessageType MsgType;
        uint32_t BodySize;

        friend net::Buffer &operator<<(net::Buffer &_buffer, const Header &_header);
        friend net::Buffer &operator>>(net::Buffer &_buffer, Header &_header);
    };
}