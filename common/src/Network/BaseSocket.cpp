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

    uint32_t BaseSocket::getPort() const
    {
        return c::Socket::getPort(m_fd);
    }

    bool BaseSocket::close()
    {
        if (!isOpen())
            return true;
        if (c::Socket::close(m_fd)) {
            m_fd = -1;
            return true;
        }
        return false;
    }

    int BaseSocket::FD() const
    {
        return m_fd;
    }

    BaseSocket::BaseSocket(int _dom, int _type, int _proto)
        : m_dom(_dom), m_type(_type), m_proto(_proto)
    {
    }

    BaseSocket::BaseSocket(int _fd)
        : m_fd(_fd)
    {
    }

    bool BaseSocket::recreate()
    {
        if (isOpen())
            if (!close())
                return false;
        return create();
    }

    bool BaseSocket::create()
    {
        m_fd = c::Socket::create(m_dom, m_type, m_proto);
        return m_fd != -1;
    }
}