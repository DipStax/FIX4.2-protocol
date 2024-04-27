#pragma once

#include <string>
#include <sys/socket.h>
#include <sys/types.h>

#include <cstdint>

#include "Common/Core/meta.hpp"

namespace net
{
    template<IsSocket T>
    class Selector;

    template<IsSocket T>
    class Acceptor;

    /// @brief C++ to C binding namespace.
    namespace c
    {
        /// @brief C++ binding class for file descriptor socket.
        class Socket
        {
            public:
                Socket() = default;
                Socket(int _dom, int _type, int _proto);
                ~Socket();

                /// @brief C++ binding to the C function socket.
                /// @param _dom The familly of protocol uesd.
                /// @param _type The type of logique for communication.
                /// @param _proto Specific protocol type from the protocol familly.
                /// @return The file descriptor created.
                [[nodiscard]] static int create(int _dom, int _type, int _proto);

                /// @brief Accept an incoming connection.
                /// @param _fd Socket from where the connection is request.
                /// @return The file descriptor of the incoming connection.
                [[nodiscard]] static int accept(int _fd);

                /// @brief Send data to an endpoint.
                /// @param _fd File descriptor to send to.
                /// @param _data Data to send to the endpoint.
                /// @param _size Size in bytes of data to send.
                /// @return The size send to the endpoint, if an error occured it return -1.
                static size_t send(int _fd, const uint8_t *_data, size_t _size);

                /// @brief  C++ binding to the C function recv.
                /// @param _fd File descriptor of the socket receiving.
                /// @param _size Size of the data that should be receive at max.
                /// @param _error return value of the recv function, if it's set to -1 this is an error.
                /// @return The data receive (it should be delete using delete[])
                [[nodiscard]] static const uint8_t *receive(int _fd, size_t _size, int &_error);

                [[nodiscard]] const uint8_t *receiveUDP(int _fd, size_t _size, int &_error);

                /// @brief Set a file descriptor as blocking or not.
                /// @param _fd File descriptor to modify
                /// @param _block If true the file descriptor will be blocking otherwise it will be non-blocking.
                /// @return True if the modification of the file descriptor succed otherwise false.
                static bool blocking(int _fd, bool _block);
                /// @brief Retreive the blocking policy of a file descriptor.
                /// @param _fd File descriptor to check on.
                /// @return True if it is blocking otherwise false.
                [[nodiscard]] static bool isBlocking(int _fd);
                /// @brief Retreive the blocking policy of a socket.
                /// @return True if it is blocking otherwise false.
                [[nodiscard]] bool isBlocking() const;

                /// @brief C++ binding to the C function close.
                /// @param _fd File descriptor to close.
                /// @return True if the close function succed.
                static bool close(int _fd);

                /// @brief Retrevei the port of the socket.
                /// @return The port of the socket.
                [[nodiscard]] uint32_t getPort() const;
                /// @brief C++ binding from C method to retrevei the port.
                /// @param _fd File descriptor of wich we wan't the port.
                /// @return The port of the socket.
                [[nodiscard]] static uint32_t getPort(int _fd);

                /// @brief Check if the socket is connected to an endpoint.
                /// @return True if the socket is connceted to an endpoint otherwise false.
                [[nodiscard]] bool is_open();
                /// @brief C++ binding to the C method to check if a file descriptor is connected to an endpoint.
                /// @param _fd File descriptor we wan't to check the connection on.
                /// @return True if the socket is connceted to an endpoint otherwise false.
                [[nodiscard]] static bool is_open(int _fd);

            protected:
                template<IsSocket T>
                friend class ::net::Selector;
                template<IsSocket T>
                friend class ::net::Acceptor;

                /// @brief Create a new socket using m_dom, m_type and m_proto and set it to m_fd.
                /// @return True if the socket is succefully created otherwise false.
                [[nodiscard]] bool c_create();

                /// @brief Bind m_fd to the address from _addr.
                /// @param _addr C formated addres to bind to.
                /// @return True if the socket is succefully bind otherwise false.
                [[nodiscard]] bool c_bind(struct sockaddr *_addr);

                /// @brief Set the socket to listening.
                /// @param _max The maximum handled socket at once.
                /// @return True if the initialisation of listen is succefully otherwise false.
                bool c_listen(int _max);

                /// @brief Connect the socket to an endpoint.
                /// @param _ip Ip of the endpoint.
                /// @param _port Port of the endpoint.
                /// @return True if the connection is succesfull otherwise false.
                [[nodiscard]] bool c_connect(const char *_ip, uint32_t _port);

                /// @brief Accept an incoming connection.
                /// @return The file description of the incoming connection or -1 if it's fail.
                [[nodiscard]] int c_accept();

                /// @brief Send an std::string to the endpoint.
                /// @param _data the string to send.
                /// @return The number of bytes send.
                size_t c_send(const std::string &_data);

                /// @brief Send raw data to the endpoint.
                /// @param _data List of bytes to send.
                /// @param _size Size of the data to send.
                /// @return The number of bytes send.
                size_t c_send(const uint8_t *_data, size_t _size);

                /// @brief Receive data from the connected pair. If the socket is non blocking it can return a nullptr value and _error will be set accordingly.
                /// @param _size Size that you wan't to receive.
                /// @param _error Error code return by underlined C function recv.
                /// @return The list of bytes receives.
                [[nodiscard]] const uint8_t *c_receive(size_t _size, int &_error);

                [[nodiscard]] const uint8_t *c_receiveUDP(size_t _size, int &_error);

                /// @brief Set the socket blocking policy.
                /// @param _block True if the socket should be blocking else false.
                /// @return If the change as been succefully apply.
                bool c_blocking(bool _block);

                /// @brief Close the socket.
                /// @return True if the close succed otherwise false.
                bool c_close();

                /// @brief Set the file descriptor to the socket.
                /// @param _fd File descriptor to set.
                void raw(int _fd);
                /// @brief Retrevei the file descriptor of a socket.
                /// @return File descriptor of the socket.
                [[nodiscard]] int raw() const;

            private:
                int m_dom = 0;      ///< Protocol familly of the file descriptor.
                int m_type = 0;     ///< Logic type of the file descriptor.
                int m_proto = 0;    ///< Specifique protcol from the procol familly used.

                int m_fd = 0;       ///< File descriptor of the socket.
        };
    }
}