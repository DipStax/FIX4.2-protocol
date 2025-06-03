#include "Common/Network/Socket.hpp"
#include "Common/Network/CSocket.hpp"

namespace net::type
{
    size_t Tcp::send(const std::string &_data)
    {
        return send(reinterpret_cast<const uint8_t *>(_data.c_str()), _data.size());
    }

    size_t Tcp::send(const uint8_t *_data, size_t _size)
    {
        return c::Socket::send(m_fd, _data, _size);
    }

    std::string Tcp::receive(size_t _size, int &_error)
    {
        const uint8_t *data = c::Socket::receive(m_fd, _size, _error);
        std::string str = "";

        if (_error == -1) {
            if (data != nullptr)
                delete[] data;
            return str;
        }
        str.assign(data, data + _error);
        delete[] data;
        return str;
    }

    Tcp::Tcp(int _type, bool _is_dom)
        : BaseSocket(_type, SOCK_STREAM, 0)
    {
        std::ignore = _is_dom;
    }

    Tcp::Tcp(int _fd)
        : BaseSocket(_fd)
    {
        // verify the correct type
    }
}