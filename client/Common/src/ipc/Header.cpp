#include "Client/Common/ipc/Header.hpp"

namespace ipc
{
    net::Buffer &operator<<(net::Buffer &_buffer, const Header &_header)
    {
        return _buffer << static_cast<uint8_t>(_header.MsgType) << _header.BodySize;
    }

    net::Buffer &operator>>(net::Buffer &_buffer, Header &_header)
    {
        uint8_t msgtype;

        _buffer >> msgtype >> _header.BodySize;
        _header.MsgType = static_cast<MessageType>(msgtype);
        return _buffer;
    }
}