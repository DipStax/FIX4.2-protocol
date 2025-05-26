#include <cstring>

#include <string.h>
#include <netinet/in.h>

#include "Common/Network/Acceptor.hpp"

namespace net
{
    template<IsSocket T>
    Acceptor<T>::Acceptor()
        : c::Socket(AF_INET, SOCK_STREAM, 0) // change for TCP or UDP
    {
    }

    template<IsSocket T>
    Acceptor<T>::~Acceptor()
    {
        (void)close();
    }

    template<IsSocket T>
    void Acceptor<T>::blocking(bool _block)
    {
        (void)c_blocking(_block);
    }

    template<IsSocket T>
    bool Acceptor<T>::listen(uint32_t _port)
    {
        struct sockaddr_in addr;

        std::memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(_port);

        if (!c_create())
            return false;
        if (!c_bind((struct sockaddr *)&addr)) {
            (void)c_close();
            return false;
        }
        if (!c_listen(MAX_SOCKET)) {
            (void)c_close();
            return false;
        }
        return true;
    }

    template<IsSocket T>
    Acceptor<T>::Client Acceptor<T>::accept()
    {
        int fd = c_accept();
        Client socket = nullptr;

        if (fd == -1)
            return nullptr;
        socket = std::make_shared<T>();
        socket->raw(fd);
        return socket;
    }

    template<IsSocket T>
    bool Acceptor<T>::close()
    {
        return c_close();
    }
}