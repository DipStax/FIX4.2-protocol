#include <cstring>

#include <string.h>
#include <netinet/in.h>

#include "Common/Network/Acceptor.hpp"
#include "Common/Network/CSocket.hpp"

namespace net
{
    template<IsSocketDomain T>
    Acceptor<T>::Acceptor()
        : BaseSocket(T::Domain, T::Type, 0) // change for TCP or UDP
    {
    }

    template<IsSocketDomain T>
    bool Acceptor<T>::listen(uint32_t _port)
    {
        struct sockaddr_in addr;

        std::memset(&addr, 0, sizeof(addr));
        addr.sin_family = T::Domain;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(_port);

        if (create())
            return false;
        if (!c::Socket::bind(m_fd, reinterpret_cast<struct sockaddr *>(&addr))) {
            close();
            return false;
        }
        if (!c::Socket::listen(m_fd, MAX_SOCKET)) {
            close();
            return false;
        }
        return true;
    }

    template<IsSocketDomain T>
    Acceptor<T>::Client Acceptor<T>::accept()
    {
        std::cout << "log here"<< std::endl;
        int fd = c::Socket::accept(m_fd);
        std::cout << "log here"<< std::endl;
        Client socket = nullptr;

        if (fd == -1)
            return nullptr;
        socket = std::make_shared<T>(fd);
        return socket;
    }
}