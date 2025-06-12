#include "Client/Common/ipc/Header.hpp"

namespace ipc
{
    net::Buffer &operator<<(net::Buffer &_buffer, const Header &_header)
    {
        return _buffer << _header.MsgType << _header.BodySize;
    }

    net::Buffer &operator>>(net::Buffer &_buffer, Header &_header)
    {
        _buffer >> _header.MsgType >> _header.BodySize;
        return _buffer;
    }
}