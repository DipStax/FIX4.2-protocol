#pragma once

#include <string>
#include <sys/socket.h>
#include <sys/types.h>

#include <cstdint>

#include "Common/meta.hpp"

/// @brief C to C++ binding namespace.
namespace net::c
{
    /// @brief C++ binding class for file descriptor socket.
    class Socket
    {
        public:
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
            /// @brief  C++ binding to the C function recvfrom.
            /// @param _fd File descriptor of the socket receiving.
            /// @param _size Size of the data that should be receive at max.
            /// @param _error return value of the recv function, if it's set to -1 this is an error.
            /// @return The data receive (it should be delete using delete[])
            [[nodiscard]] static const uint8_t *receiveUDP(int _fd, size_t _size, int &_error);

            /// @brief Set a file descriptor as blocking or not.
            /// @param _fd File descriptor to modify
            /// @param _block If true the file descriptor will be blocking otherwise it will be non-blocking.
            /// @return True if the modification of the file descriptor succed otherwise false.
            static bool setBlocking(int _fd, bool _block);
            /// @brief Retreive the blocking policy of a file descriptor.
            /// @param _fd File descriptor to check on.
            /// @return True if it is blocking otherwise false.
            [[nodiscard]] static bool isBlocking(int _fd);

            /// @brief C++ binding to the C function close.
            /// @param _fd File descriptor to close.
            /// @return True if the close function succed.
            static bool close(int _fd);

            /// @brief C++ binding from C method to retrevei the port.
            /// @param _fd File descriptor of wich we wan't the port.
            /// @return The port of the socket.
            [[nodiscard]] static uint32_t getPort(int _fd);

            /// @brief C++ binding to the C method to check if a file descriptor is connected to an endpoint.
            /// @param _fd File descriptor we wan't to check the connection on.
            /// @return True if the socket is connceted to an endpoint otherwise false.
            [[nodiscard]] static bool is_open(int _fd);

            /// @brief Bind an address to a socket
            /// @param _fd File descriptor to bind.
            /// @param _addr Address to bind to
            /// @return True if the bind function succed.
            static bool bind(int _fd, struct sockaddr *_addr);

            /// @brief C++ binding to the C function listen.
            /// @param _fd File descirptor wich listen.
            /// @param _max Maximum number of waiting socket.
            /// @return Truye if the listen function succed.
            static bool listen(int _fd, int _max);

            static bool connect(int _fd, struct sockaddr *_addr, size_t _size);

            static bool reusePort(int _fd, bool _flag);
    };
}