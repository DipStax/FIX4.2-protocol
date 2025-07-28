#include "Client/Shared/IPC/MessageType.hpp"

namespace ipc
{
    net::Buffer &operator<<(net::Buffer &_buffer, const MessageType &_type)
    {
        return _buffer << static_cast<uint8_t>(_type);
    }

    net::Buffer &operator>>(net::Buffer &_buffer, MessageType &_type)
    {
        uint8_t type;
        _buffer >> type;
        _type = static_cast<MessageType>(type);
        return _buffer;
    }
}