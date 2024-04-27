#include <cstring>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h> 

#include "Common/Core/Logger.hpp"
#include "Common/Network/Socket.hpp"

namespace net
{
    void Socket::blocking(bool _block)
    {
        (void)c_blocking(_block);
    }

    bool Socket::connect(const Ip &_ip, uint32_t _port)
    {
        if (!c_create())
            return false;
        return c_connect(_ip.c_str(), _port);
    }

    size_t Socket::send(const std::string &_data)
    {
        return c_send(reinterpret_cast<const uint8_t *>(_data.c_str()), _data.size());
    }

    size_t Socket::send(const uint8_t *_data, size_t _size)
    {
        return c_send(_data, _size);
    }

    std::string Socket::receive(size_t _size, int &_error)
    {
        const uint8_t *data = c_receive(_size, _error);
        std::string str = "";

        if (_error == -1) {
            Logger::Log("[Socket] Error will trying to receive data: ", strerror(errno));
            if (data != nullptr)
                delete[] data;
            return str;
        }
        str.assign(data, data + _error);
        delete[] data;
        return str;
    }

    std::string Socket::receiveUDP(size_t _size, int &_error)
    {
        const uint8_t *data = c_receiveUDP(_size, _error);
        std::string str = "";

        if (_error == -1) {
            Logger::Log("[Socket] Error will trying to receive data: ", strerror(errno));
            if (data != nullptr)
                delete[] data;
            return str;
        }
        str.assign(data, data + _error);
        delete[] data;
        return str;
    }

    bool Socket::close()
    {
        return c_close();
    }

    Socket::operator bool()
    {
        return is_open();
    }

    bool Socket::operator==(const Socket &_socket)
    {
        return raw() == _socket.raw();
    }


    Socket::Socket(int _type)
        : c::Socket(AF_INET, _type, 0)
    {
    }

    namespace tcp
    {
        Socket::Socket()
            : ::net::Socket(SOCK_STREAM)
        {
            Logger::Log("[tcp::Socket] New TCP socket");
        }

    }

    namespace udp
    {
        Socket::Socket()
            : ::net::Socket(SOCK_DGRAM)
        {
            Logger::Log("[udp::Socket] New UDP socket");
        }

        bool Socket::broadcasting() const
        {
            return m_broadcast;
        }

        bool Socket::broadcastOn(uint32_t _port)
        {
            int enable = 1;

            if (is_open()) {
                close();
            }
            if (!c_create())
                return false;
            if (setsockopt(raw(), SOL_SOCKET, SO_BROADCAST, &enable, sizeof(enable)) == -1) {
                Logger::Log("[udp::Socket] Failed to set the broadcast flag: ", strerror(errno));
                close();
                return false;
            }

            std::memset(&m_broad_addr, 0, sizeof(m_broad_addr));
            m_broad_addr.sin_family = AF_INET;
            m_broad_addr.sin_port = htons(8081);
            m_broad_addr.sin_addr.s_addr =  htonl(INADDR_LOOPBACK);
            m_broadcast = true;

            return true;
        }

        bool Socket::broadcast(const uint8_t *_data, size_t _size)
        {
            if (sendto(raw(), _data, _size, 0, (struct sockaddr*)&m_broad_addr, sizeof(sockaddr_in)) == -1) {
                Logger::Log("[udp::Socket] Failed to send the data on the broadcast: ", strerror(errno));
                return false;
            }
            return true;
        }

        bool Socket::bind() {

            // struct sockaddr_in addr_client = getAddr();

            return c_bind((struct sockaddr *)&m_broad_addr);
        }
    }
}