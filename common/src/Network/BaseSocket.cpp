#include "Common/Network/BaseSocket.hpp"
#include "Common/Network/CSocket.hpp"

namespace net
{
    BaseSocket::~BaseSocket()
    {
        close();
    }

    bool BaseSocket::setBlocking(bool _flag)
    {
        return c::Socket::setBlocking(m_fd, _flag);
    }

    bool BaseSocket::isBlocking() const
    {
        return c::Socket::isBlocking(m_fd);
    }

    bool BaseSocket::isOpen() const
    {
        bool block = isBlocking();

        if (!c::Socket::setBlocking(m_fd, false) || !c::Socket::is_open(m_fd))
            return false;
        return c::Socket::setBlocking(m_fd, block);
    }

    BaseSocket::operator bool() const
    {
        return isOpen();
    }

    bool BaseSocket::close()
    {
        return c::Socket::close(m_fd);
    }

    BaseSocket::BaseSocket(int _dom, int _type, int _proto)
        : m_dom(_dom), m_type(_type), m_proto(_proto)
    {
        recreate();
    }

    BaseSocket::BaseSocket(int _fd)
        : m_fd(_fd)
    {
    }

    int BaseSocket::recreate()
    {
        m_fd = c::Socket::create(m_dom, m_type, m_proto);
    }
}