#include <cstring>

#include <arpa/inet.h>

#include "Shared/Network/Socket.hpp"
#include "Shared/Network/CSocket.hpp"

namespace net::dom
{
    bool INet::connect(const Ip &_ip, uint32_t _port)
    {
        std::memset(&m_addr, 0, sizeof(struct sockaddr_in));
        m_addr.sin_family = Domain;
        m_addr.sin_port = htons(_port);
        this->recreate();
        if (inet_pton(Domain, _ip.c_str(), &m_addr.sin_addr) <= 0)
            return false;
        return c::Socket::connect(this->FD(), reinterpret_cast<struct sockaddr *>(&m_addr), sizeof(struct sockaddr_in));
    }

    bool INet::connect(uint32_t _ip, uint32_t _port)
    {
        std::memset(&m_addr, 0, sizeof(struct sockaddr_in));
        m_addr.sin_family = Domain;
        m_addr.sin_port = htons(_port);
        m_addr.sin_addr.s_addr = htonl(_ip);

        this->recreate();
        return c::Socket::connect(this->FD(), reinterpret_cast<struct sockaddr *>(&m_addr), sizeof(struct sockaddr_in));
    }

    INet::INet(int _type, int _proto)
        : BaseSocket(Domain, _type, _proto)
    {
    }

    INet::INet(int _fd)
        : BaseSocket(_fd)
    {
        // verify correct domain
    }

    bool Unix::connect(const std::string &_path)
    {
        this->m_addr.sun_family = AF_UNIX;
        std::strncpy(this->m_addr.sun_path, _path.c_str(), sizeof(this->m_addr.sun_path) - 1);

        this->recreate();
        return c::Socket::connect(this->FD(), reinterpret_cast<struct sockaddr *>(&this->m_addr), sizeof(sockaddr_un));
    }

    bool Unix::unlink()
    {
        if (std::strlen(this->m_addr.sun_path) != 0)
            return ::unlink(this->m_addr.sun_path);
        return false;
    }

    Unix::Unix(int _type, int _proto)
        : BaseSocket(Domain, _type, _proto)
    {
    }

    Unix::Unix(int _fd)
        : BaseSocket(_fd)
    {
        // verify correct domain
    }
}