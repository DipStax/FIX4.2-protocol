#pragma once

#include <string>

#include <stdint.h>

namespace net
{
    /// @brief Class for ip address manipulation
    class Ip
    {
        public:
            Ip();
            /// @brief Construct the ip address using the possible net::Ip::set function.
            /// @tparam ...Args determine wich net::Ip::set function to call.
            /// @param ..._args the value transfered to the related net::Ip::set function.
            template<class ...Args>
            Ip(Args &&..._args);
            ~Ip() = default;

            /// @brief Set the ip with a string
            /// @param _ip ip address formated as "127.0.0.1"
            void set(const std::string& _ip);
            /// @brief Set the ip using 4 bytes
            /// @param _ip Ip address formated as 0xff000001
            void set(uint32_t _ip);
            /// @brief Set the ip using 4 differents byte
            /// @param _b1 First byte of the ip addres 0xff
            /// @param _b3 Second byte of the ip address 0x00
            /// @param _b2 Third byte of the ip address 0x00
            /// @param _b4 Fourth byte if the ip address 0x01
            void set(uint8_t _b1, uint8_t _b3, uint8_t _b2, uint8_t _b4);

            /// @brief Get the ip address on a c-string format.
            /// @return The ip address.
            [[nodiscard]] const char *c_str() const;

            /// @brief Get the uo address on a C++ string format.
            operator std::string() const;

        private:
            std::string m_ip{};     ///< String containing the ip address formated as "127.0.0.1".
    };
}

#include "Common/Network/Ip.inl"