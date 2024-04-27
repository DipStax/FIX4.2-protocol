#include <cstring>

#include <string.h>
#include <netinet/in.h>

#include "Common/Core/Logger.hpp"
#include "Common/Network/Acceptor.hpp"

namespace net
{
    template<IsSocket T>
    Acceptor<T>::Acceptor()
        : c::Socket(AF_INET, SOCK_STREAM, 0) // change for TCP or UDP
    {
        Logger::Log("[Acceptor] New acceptor");
    }

    template<IsSocket T>
    Acceptor<T>::~Acceptor()
    {
        Logger::Log("[Acceptor] Closing acceptor");
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

        Logger::Log("[Acceptor] Initialisation of new listner");
        std::memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(_port);

        Logger::Log("[Acceptor] Create new listner socket");
        if (!c_create()) {
            Logger::Log("[Acceptor] Failed to create socket: ", strerror(errno));
            return false;
        }
        Logger::Log("[Acceptor] Bind new listner socket");
        if (!c_bind((struct sockaddr *)&addr)) {
            Logger::Log("[Acceptor] Bind failed: ", strerror(errno));
            (void)c_close();
            return false;
        }
        Logger::Log("[Acceptor] Initialisation of the listner on port: ", _port);
        if (!c_listen(MAX_SOCKET)) {
            Logger::Log("[Acceptor] Listening failed: ", strerror(errno));
            (void)c_close();
            return false;
        }
        Logger::Log("[Acceptor] New listener on port: ", _port);
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