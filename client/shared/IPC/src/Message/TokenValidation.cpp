#include "Client/Shared/IPC/Message/TokenValidation.hpp"

#include "Shared/Utils/Utils.hpp"

namespace ipc::msg
{
    net::Buffer &operator<<(net::Buffer &_buffer, const InitiatorToFrontValidToken &_validation)
    {
        return _buffer << _validation.token << _validation.port;
    }

    net::Buffer &operator>>(net::Buffer &_buffer, InitiatorToFrontValidToken &_validation)
    {
        return _buffer >> _validation.token >> _validation.port;
    }

    std::ostream &operator<<(std::ostream &_os, const InitiatorToFrontValidToken &_validation)
    {
        return _os << "{ Token: " << utils::trunc(_validation.token) << ", Port: " << _validation.port << " }";
    }

    net::Buffer &operator<<(net::Buffer &_buffer, const FrontToBackValidToken &_validation)
    {
        return _buffer << _validation.token;
    }

    net::Buffer &operator>>(net::Buffer &_buffer, FrontToBackValidToken &_validation)
    {
        return _buffer >> _validation.token;
    }

    std::ostream &operator<<(std::ostream &_os, const FrontToBackValidToken &_validation)
    {
        return _os << "{ Token: " << utils::trunc(_validation.token) << " }";
    }
}