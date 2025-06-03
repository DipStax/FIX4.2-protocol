#include "Common/Network/CSocket.hpp"

namespace net::dom
{
    template<IsSocketType T>
    INet::INet()
        : T(Domain, true)
    {
    }

    template<IsSocketType T>
    INet::INet(int _fd)
        : T(_fd)
    {
        // verify correct domain
    }

    template<IsSocketType T>
    bool INet::connect(const Ip &_ip, uint32_t _port)
    {
        struct sockaddr_in addr;

        addr.sin_family = m_dom;
        addr.sin_port = htons(_port);
        if (inet_pton(m_dom, _ip.c_str(), &addr.sin_addr) <= 0)
            return false;
        if (connect(m_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
            return false;
        return true;
    }

    template<IsSocketType T>
    bool INet::connect(uint32_t _ip, uint32_t _port)
    {
        struct sockaddr_in addr;

        addr.sin_family = m_dom;
        addr.sin_port = htons(_port);
        addr.sin_addr.s_addr = htonl(_ip);
        if (connect(m_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
            return false;
        return true;
    }

    template<IsSocketType T>
    Unix::Unix()
        : T(Domain, true)
    {
    }

    template<IsSocketType T>
    Unix::Unix(int _fd)
        : T(_fd)
    {
        // verify correct domain
    }

    template<IsSocketType T>
    bool Unix::connect(const std::string &_path)
    {
        sockaddr_un addr{};
        addr.sun_family = AF_UNIX;
        std::strncpy(addr.sun_path, _path.c_str(), sizeof(addr.sun_path) - 1);

        if (connect(m_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
            return false;
        return true;
    }
}