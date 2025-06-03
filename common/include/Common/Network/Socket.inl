#include <arpa/inet.h>
#include <sys/un.h>
#include <cstring>

#include "Common/Network/CSocket.hpp"

namespace net::dom
{
    template<IsSocketType T>
    INet<T>::INet()
        : T(Domain, true)
    {
    }

    template<IsSocketType T>
    INet<T>::INet(int _fd)
        : T(_fd)
    {
        // verify correct domain
    }

    template<IsSocketType T>
    bool INet<T>::connect(const Ip &_ip, uint32_t _port)
    {
        struct sockaddr_in addr;

        addr.sin_family = Domain;
        addr.sin_port = htons(_port);
        if (this->isOpen())
            this->close();
        this->create();
        if (inet_pton(Domain, _ip.c_str(), &addr.sin_addr) <= 0)
            return false;
        if (::connect(this->m_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
            return false;
        return true;
    }

    template<IsSocketType T>
    bool INet<T>::connect(uint32_t _ip, uint32_t _port)
    {
        struct sockaddr_in addr;

        std::memset(&addr, 0, sizeof(addr));
        addr.sin_family = Domain;
        addr.sin_port = htons(_port);
        addr.sin_addr.s_addr = htonl(_ip);
        return c::Socket::connect(this->m_fd, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) == 0;
    }

    template<IsSocketType T>
    Unix<T>::Unix()
        : T(Domain, true)
    {
    }

    template<IsSocketType T>
    Unix<T>::Unix(int _fd)
        : T(_fd)
    {
        // verify correct domain
    }

    template<IsSocketType T>
    bool Unix<T>::connect(const std::string &_path)
    {
        sockaddr_un addr{};
        addr.sun_family = AF_UNIX;
        std::strncpy(addr.sun_path, _path.c_str(), sizeof(addr.sun_path) - 1);

        if (connect(this->m_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
            return false;
        return true;
    }
}