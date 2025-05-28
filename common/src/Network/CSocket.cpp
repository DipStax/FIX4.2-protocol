#include <cstddef>

#include <arpa/inet.h>
#include <fcntl.h>

#include "Common/Network/CSocket.hpp"

namespace net::c
{
    int Socket::create(int _dom, int _type, int _proto)
    {
        return socket(_dom, _type, _proto);
    }

    int Socket::accept(int _fd)
    {
        return ::accept(_fd, (struct sockaddr *)NULL, NULL);
    }

    size_t Socket::send(int _fd, const uint8_t *_data, size_t _size)
    {
        return ::send(_fd, _data, _size, 0);
    }

    const uint8_t *Socket::receiveUDP(int _fd, size_t _size, struct sockaddr *_addr, int &_error)
    {
        std::unique_ptr<uint8_t []> data(new uint8_t[_size]);
        socklen_t addr_len = sizeof(_addr);

        _error = ::recvfrom(_fd, data.get(), _size, 0, _addr, &addr_len);
        return data.release();
    }

    const uint8_t *Socket::receive(int _fd, size_t _size, int &_error)
    {
        std::unique_ptr<uint8_t []> data(new uint8_t[_size]{});

        _error = ::recv(_fd, data.get(), _size, 0);
        return data.release();
    }

    bool Socket::setBlocking(int _fd, bool _block)
    {
        int flags = fcntl(_fd, F_GETFL, 0);

        if (flags == -1)
            return false;
        return fcntl(_fd, F_SETFL, _block ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK)) == 0;
    }

    bool Socket::isBlocking(int _fd)
    {
        return fcntl(_fd, F_GETFL, 0) & O_NONBLOCK;
    }

    bool Socket::close(int _fd)
    {
        if (::close(_fd) != 0)
            return false;
        return true;
    }

    uint32_t Socket::getPort(int _fd)
    {
        struct sockaddr_in sin;
        socklen_t len = sizeof(sin);

        if (getsockname(_fd, (struct sockaddr *)&sin, &len) == -1)
            return 0;
        return ntohs(sin.sin_port);
    }

    bool Socket::is_open(int _fd)
    {
        return fcntl(_fd, F_GETFD) != -1;
    }

    bool Socket::bind(int _fd, struct sockaddr *_addr)
    {
        return ::bind(_fd, _addr, sizeof(struct sockaddr_in)) == 0;
    }

     bool Socket::listen(int _fd, int _max)
    {
        return ::listen(_fd, _max) == 0;
    }

    bool Socket::connect(int _fd, struct sockaddr *_addr, size_t _size)
    {
        return ::connect(_fd, _addr, _size) == 0;
    }

    bool Socket::setReusePort(int _fd, bool _flag)
    {
        return setsockopt(_fd, SOL_SOCKET, SO_REUSEPORT, &_flag, sizeof(bool)) == 0;
    }

    bool Socket::setBroadcast(int _fd, bool _bc)
    {
        int bc_cast = static_cast<int>(_bc);

        addr.sin_family = m_dom;
        addr.sin_port = htons(_port);
        if (inet_pton(m_dom, _ip, &addr.sin_addr) <= 0)
            return false;
        if (connect(m_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
            return false;
        return true;
    }

    int Socket::c_accept()
    {
        return accept(m_fd);
    }

    size_t Socket::c_send(const std::string &_data)
    {
        return c_send(reinterpret_cast<const uint8_t *>(_data.c_str()), _data.size());
    }

    size_t Socket::c_send(const uint8_t *_data, size_t _size)
    {
        return send(m_fd, _data, _size);
    }

    const uint8_t *Socket::c_receive(size_t _size, int &_error)
    {
        return receive(m_fd, _size, _error);
    }

    const uint8_t *Socket::c_receiveUDP(size_t _size, int &_error)
    {
        return receiveUDP(m_fd, _size, _error);
    }

    bool Socket::c_blocking(bool _block)
    {
        return blocking(m_fd, _block);
    }

    bool Socket::c_close()
    {
        // if (!is_open())
        //     return true;
        return close(m_fd);
    }

    void Socket::raw(int _fd)
    {
        m_fd = _fd;
    }

    int Socket::raw() const
    {
        return m_fd;
    }
}
