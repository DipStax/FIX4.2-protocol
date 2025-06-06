#include <cstring>
#include <string.h>

#include <netinet/in.h>
#include <unistd.h>

#include "Common/Network/Acceptor.hpp"
#include "Common/Network/CSocket.hpp"

namespace net
{
    template<IsAcceptorSocket T>
    Acceptor<T>::Acceptor()
        : BaseSocket(T::Domain, T::Type, 0) // change for TCP or UDP
    {
    }


    template<IsAcceptorSocket T>
    Acceptor<T>::~Acceptor()
    {
        if constexpr (IsUnixSocketDomain<T>)
            this->unlink();
    }

    template<IsAcceptorSocket T>
    bool Acceptor<T>::listen(uint32_t _port)
        requires IsINetSocketDomain<T>
    {
        struct sockaddr_in addr;

        std::memset(&addr, 0, sizeof(addr));
        addr.sin_family = T::Domain;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(_port);

        if (!recreate())
            return false;
        if (!c::Socket::bind(this->FD(), reinterpret_cast<struct sockaddr *>(&addr))) {
            close();
            return false;
        }
        c::Socket::setReusePort(this->FD(), true);
        if (!c::Socket::listen(this->FD(), MAX_SOCKET)) {
            close();
            return false;
        }
        return true;
    }

    template<IsAcceptorSocket T>
    bool Acceptor<T>::listen(const std::string &_path)
        requires IsUnixSocketDomain<T>
    {
        struct sockaddr_un addr;

        if (_path.size() >= sizeof(addr.sun_path))
            return false;

        if (!this->unlink())
            return false;
        this->m_path = _path;
        std::memset(&addr, 0, sizeof(addr));
        addr.sun_family = T::Domain;
        std::strncpy(addr.sun_path, this->m_path.c_str(), sizeof(addr.sun_path) - 1);

        if (!recreate())
            return false;
        if (!c::Socket::bind(this->FD(), reinterpret_cast<struct sockaddr *>(&addr))) {
            close();
            return false;
        }
        if (!c::Socket::listen(this->FD(), MAX_SOCKET)) {
            close();
            return false;
        }
        return true;
    }

    template<IsAcceptorSocket T>
    Acceptor<T>::Client Acceptor<T>::accept()
    {
        int fd = c::Socket::accept(this->FD());
        Client socket = nullptr;

        if (fd == -1)
            return nullptr;
        socket = std::make_shared<T>(fd);
        return socket;
    }

    template<IsAcceptorSocket T>
    bool Acceptor<T>::unlink()
        requires IsUnixSocketDomain<T>
    {
        if (this->m_path.empty())
            return true;
        return ::unlink(this->m_path.c_str()) == 0;
    }
}