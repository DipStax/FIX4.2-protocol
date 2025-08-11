#include "Client/Shared/IPC/Message/Identify.hpp"

namespace ipc::msg
{
    net::Buffer &operator<<(net::Buffer &_buffer, const IdentifyFront &_identify)
    {
        return _buffer << _identify.apiKey;
    }

    net::Buffer &operator>>(net::Buffer &_buffer, IdentifyFront &_identify)
    {
        return _buffer >> _identify.apiKey;
    }

    std::ostream &operator<<(std::ostream &_os, const IdentifyFront &_identify)
    {
        return _os << "{ ApiKey: " << _identify.apiKey << " }";
    }
}