#include "Common/Network/Ip.hpp"

namespace net
{
    Ip::Ip()
        : m_ip("0.0.0.0")
    {
    }

    void Ip::set(const std::string& _ip)
    {
        m_ip = _ip;
    }

    void Ip::set(uint32_t _ip)
    {
        set(_ip & 0xf, (_ip >> 0x8) & 0xf, (_ip >> 0xf) & 0xf, (_ip >> 0x18) & 0xf);
    }

    void Ip::set(uint8_t _b1, uint8_t _b2, uint8_t _b3, uint8_t _b4)
    {
        set(std::to_string(_b1) + "." +
            std::to_string(_b2) + "." +
            std::to_string(_b3) + "." +
            std::to_string(_b4));
    }

    const char *Ip::c_str() const
    {
        return m_ip.c_str();
    }

    Ip::operator std::string() const
    {
        return m_ip;
    }
}