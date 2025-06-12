#pragma once

#include <cstdint>

#include "Client/Common/ipc/MessageType.hpp"

namespace ipc
{
    struct Header
    {
        MessageType MsgType;
        uint32_t BodySize;

        friend net::Buffer &operator<<(net::Buffer &_buffer, const Header &_header);
        friend net::Buffer &operator>>(net::Buffer &_buffer, Header &_header);
    };
}