#include "Client/Shared/IPC/Message/Identify.hpp"

namespace ipc::msg
{
    net::Buffer &operator<<(net::Buffer &_buffer, const IdentifyFront &_identify)
    {
        return _buffer << _identify.apiKey << _identify.backport;
    }

    net::Buffer &operator>>(net::Buffer &_buffer, IdentifyFront &_identify)
    {
        return _buffer >> _identify.apiKey >> _identify.backport;
    }

    std::ostream &operator<<(std::ostream &_os, const IdentifyFront &_identify)
    {
        return _os << "{ ApiKey: " << _identify.apiKey << " BackPort: " << _identify.backport <<" }";
    }
}