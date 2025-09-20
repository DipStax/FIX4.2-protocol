#include "Client/Shared/IPC/Message/Reject.hpp"

namespace ipc::msg
{
    net::Buffer &operator<<(net::Buffer &_buffer, const Reject &reject)
    {
        return _buffer << reject.message;
    }

    net::Buffer &operator>>(net::Buffer &_buffer, Reject &reject)
    {
        return _buffer >> reject.message;
    }

    std::ostream &operator<<(std::ostream &_os, const Reject &reject)
    {
        return _os << "{ Message: " << reject.message << " }";
    }
}