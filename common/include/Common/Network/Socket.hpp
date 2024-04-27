#pragma once

#include <string>
#include <type_traits>

#include <netinet/in.h>

#include "Common/Network/CSocket.hpp"
#include "Common/Network/Ip.hpp"
#include "Common/Thread/Queue.hpp"

namespace net
{
    class Socket : public c::Socket
    {
        public:
            using Output = ts::Queue<const uint8_t *>;

            ~Socket() = default;

            bool connect(const Ip &_ip, uint32_t _port);
            bool connect(uint32_t _ip, uint32_t _port);

            void blocking(bool _block);
            [[nodiscard]] bool blocking() const;

            size_t send(const std::string &_data);
            size_t send(const uint8_t *_data, size_t _size);

            [[nodiscard]] std::string receive(size_t _size, int &_error);

            [[nodiscard]] std::string receiveUDP(size_t _size, int &_error);

            bool close();

            operator bool();

            bool operator==(const Socket &_socket);

        protected:
            Socket(int _type);

        private:
            int m_type = 0;
    };

    namespace tcp
    {
        class Socket : public ::net::Socket
        {
            public:
                Socket();
                ~Socket() = default;
        };
    }

    namespace udp
    {
        class Socket : public ::net::Socket
        {
            public:
                Socket();
                ~Socket() = default;

                /// @brief Check if the socket is in broadcasting mode.
                /// @return True if it is, otherwise false.
                [[nodiscard]] bool broadcasting() const;
                /// @brief Start broading on a port.
                /// @param _port Port to broadcast from.
                /// @return False if the setup of the broadcast failed, otherwise true.
                [[nodiscard]] bool broadcastOn(uint32_t _port);
                /// @brief Broadcast data.
                /// @param _data Data to send.
                /// @param _size Size of the data to send.
                /// @return False if the broadcast failed, otherwise true.
                [[nodiscard]] bool broadcast(const uint8_t *_data, size_t _size);
                
                [[nodiscard]] bool bind();

            private:
                bool m_broadcast = false;               ///< Broadcast mode.

                struct sockaddr_in m_broad_addr;        ///< Broadcast address.
        };
    }
}