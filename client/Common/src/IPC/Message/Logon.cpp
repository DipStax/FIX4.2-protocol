#include "Client/Common/IPC/Message/Logon.hpp"

namespace ipc::msg
{
    net::Buffer &operator<<(net::Buffer &_buffer, const Logon &_logon)
    {
        return _buffer << _logon.UserId << _logon.SeqNum << _logon.HeartBeat;
    }

    net::Buffer &operator>>(net::Buffer &_buffer, Logon &_logon)
    {
        return _buffer >> _logon.UserId >> _logon.SeqNum >> _logon.HeartBeat;
    }

    std::ostream &operator<<(std::ostream &_os, const Logon &_logon)
    {
        return _os << "{ UserId: " << _logon.UserId << ", SeqNum: "<< _logon.SeqNum << ", HeartBeat: "<< _logon.HeartBeat << " }";
    }
}