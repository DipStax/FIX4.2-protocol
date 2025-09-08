#include "Client/Shared/IPC/Message/Authentification.hpp"

namespace ipc::msg
{
    net::Buffer &operator<<(net::Buffer &_buffer, const AuthFrontToInitiator &_auth)
    {
        return _buffer << _auth.apikey;
    }

    net::Buffer &operator>>(net::Buffer &_buffer, AuthFrontToInitiator &_auth)
    {
        return _buffer >> _auth.apikey;
    }

    std::ostream &operator<<(std::ostream &_os, const AuthFrontToInitiator &_auth)
    {
        return _os << "{ ApiKey: " << _auth.apikey <<" }";
    }

    net::Buffer &operator<<(net::Buffer &_buffer, const AuthFrontToBack &_auth)
    {
        return _buffer << _auth.token;
    }

    net::Buffer &operator>>(net::Buffer &_buffer, AuthFrontToBack &_auth)
    {
        return _buffer >> _auth.token;
    }

    std::ostream &operator<<(std::ostream &_os, const AuthFrontToBack &_auth)
    {
        return _os << "{ Token: " << _auth.token << " }";
    }

    net::Buffer &operator<<(net::Buffer &_buffer, const AuthInitiatorToBack &_auth)
    {
        return _buffer << _auth.token << _auth.apikey;
    }

    net::Buffer &operator>>(net::Buffer &_buffer, AuthInitiatorToBack &_auth)
    {
        return _buffer >> _auth.token >> _auth.apikey;
    }

    std::ostream &operator<<(std::ostream &_os, const AuthInitiatorToBack &_auth)
    {
        return _os << "{ Token: " << _auth.token << ", ApiKey: " << _auth.apikey << "}";
    }

    net::Buffer &operator<<(net::Buffer &_buffer, const AuthBackToInitiator &_auth)
    {
        return _buffer << _auth.apikey << _auth.port;
    }

    net::Buffer &operator>>(net::Buffer &_buffer, AuthBackToInitiator &_auth)
    {
        return _buffer >> _auth.apikey >> _auth.port;
    }

    std::ostream &operator<<(std::ostream &_os, const AuthBackToInitiator &_auth)
    {
        return _os << "{ ApiKey: " << _auth.apikey << ", Port: " << _auth.port << "}";
    }
}