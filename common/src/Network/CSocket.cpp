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

    size_t Socket::send(int _fd, const std::byte *_data, size_t _size)
    {
        return ::send(_fd, _data, _size, 0);
    }

    size_t Socket::sendTo(int _fd, const std::byte *_data, size_t _size, struct sockaddr *_addr, size_t _addr_size)
    {
        return sendto(_fd, _data, _size, 0, _addr, _addr_size);
    }

    const std::byte *Socket::receiveFrom(int _fd, size_t _size, struct sockaddr *_addr, int &_error)
    {
        std::unique_ptr<std::byte []> data(new std::byte[_size]);
        socklen_t addr_len = sizeof(_addr);

        _error = ::recvfrom(_fd, data.get(), _size, 0, _addr, &addr_len);
        return data.release();
    }

    const std::byte *Socket::receive(int _fd, size_t _size, int &_error)
    {
        std::unique_ptr<std::byte []> data(new std::byte[_size]{});

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

    bool Socket::bind(int _fd, struct sockaddr *_addr, size_t _size)
    {
        return ::bind(_fd, _addr, _size) == 0;
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

        return setsockopt(_fd, SOL_SOCKET, SO_BROADCAST, &bc_cast, sizeof(int)) == 0;
    }
}
