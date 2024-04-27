#include <cstddef>

#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>

#include "Common/Core/Logger.hpp"
#include "Common/Network/CSocket.hpp"

namespace net::c
{
    Socket::Socket(int _dom, int _type, int _proto)
        : m_dom(_dom), m_type(_type), m_proto(_proto)
    {
        (void)c_create();
    }

    Socket::~Socket()
    {
        (void)c_close();
    }

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

    const uint8_t *Socket::receiveUDP(int _fd, size_t _size, int &_error)
    {
        std::unique_ptr<uint8_t []> data(new uint8_t[_size]);
        struct sockaddr_in addr;
        socklen_t addr_len = sizeof(addr);

        _error = ::recvfrom(_fd, data.get(), _size, 0, (struct sockaddr*)&addr, &addr_len);
        return data.release();
    }

    const uint8_t *Socket::receive(int _fd, size_t _size, int &_error)
    {
        std::unique_ptr<uint8_t []> data(new uint8_t[_size]{});

        _error = ::recv(_fd, data.get(), _size, 0);
        return data.release();
    }

    bool Socket::blocking(int _fd, bool _block)
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

    bool Socket::isBlocking() const
    {
        return isBlocking(m_fd);
    }

    bool Socket::close(int _fd)
    {
        if (::close(_fd) != 0) {
            Logger::Log("[c::Socket] Error will closing the socket: ", strerror(errno));
            return false;
        }
        return true;
    }

    uint32_t Socket::getPort() const
    {
        return getPort(m_fd);
    }

    uint32_t Socket::getPort(int _fd)
    {
        struct sockaddr_in sin;
        socklen_t len = sizeof(sin);

        if (getsockname(_fd, (struct sockaddr *)&sin, &len) == -1) {
            Logger::Log("[c::Socket] failed to retrevei the port: ", strerror(errno));
            return 0;
        }
        return ntohs(sin.sin_port);
    }

    bool Socket::is_open(int _fd)
    {
        int error = 0;
        char buffer = 0;

        error = recv(_fd, &buffer, 1, MSG_PEEK);
        if (error < 0)
            return false;
        if (error == 0 && (errno == EAGAIN || errno == EINTR))
            return true;
        return false;
    }

    bool Socket::is_open()
    {
        bool block = isBlocking();

        if (!c_blocking(false) || !is_open(m_fd))
            return false;
        return c_blocking(block);
    }

    // non static function:

    bool Socket::c_create()
    {
        m_fd = create(m_dom, m_type, m_proto);
        if (m_fd < 0) {
            Logger::Log("[c::Socket] Error will creating the socket: ", strerror(errno));
            return false;
        }
        return true;
    }

    bool Socket::c_bind(struct sockaddr *_addr)
    {
        int reuse = 1;

        if (setsockopt(m_fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(int)) != 0) {
            Logger::Log("[c::Socket] Bind: error when setting the reuse address flag: ", strerror(errno));
            return false;
        }
        if (bind(m_fd, _addr, sizeof(struct sockaddr_in)) == -1) {
            Logger::Log("[c::Socket] Bind failed: ", strerror(errno));
            return false;
        }
        return true;
    }

    bool Socket::c_listen(int _max)
    {
        if (listen(m_fd, _max) != 0) {
            Logger::Log("[c::Socket] Failed initialisation of listening: ", strerror(errno));
            return false;
        }
        return true;
    }

    bool Socket::c_connect(const char *_ip, uint32_t _port)
    {
        struct sockaddr_in addr;

        addr.sin_family = m_dom;
        addr.sin_port = htons(_port);
        if (inet_pton(m_dom, _ip, &addr.sin_addr) <= 0) {
            Logger::Log("[c::Socket] Connection failed: ", strerror(errno), ", when converting Ip");
            return false;
        }
        if (connect(m_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            Logger::Log("[c::Socket] Connection failed: ", strerror(errno));
            return false;
        }
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
        if (!is_open())
            return true;
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
