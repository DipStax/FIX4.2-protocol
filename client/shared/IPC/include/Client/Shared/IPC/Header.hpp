#pragma once

#include <cstdint>

#include "Client/Shared/IPC/MessageType.hpp"

namespace ipc
{
#pragma pack(push, 1)

    struct Header
    {
        MessageType MsgType;
        uint32_t BodySize;

        friend net::Buffer &operator<<(net::Buffer &_buffer, const Header &_header);
        friend net::Buffer &operator>>(net::Buffer &_buffer, Header &_header);
    };

#pragma pack(pop)

    static_assert(sizeof(Header) == 5, "Header is padded");
}