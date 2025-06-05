
#include "Common/Network/Socket.hpp"
#include "Common/Network/CSocket.hpp"

namespace net::type
{
    template<IsSocketDomain T>
    size_t Stream<T>::send(const std::string &_data)
    {
        return send(reinterpret_cast<const uint8_t *>(_data.c_str()), _data.size());
    }

    template<IsSocketDomain T>
    size_t Stream<T>::send(const uint8_t *_data, size_t _size)
    {
        return c::Socket::send(this->FD(), _data, _size);
    }

    template<IsSocketDomain T>
    std::string Stream<T>::receive(size_t _size, int &_error)
    {
        const uint8_t *data = c::Socket::receive(this->FD(), _size, _error);
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

    template<IsSocketDomain T>
    Stream<T>::Stream()
        : T(Type, 0)
    {
    }

    template<IsSocketDomain T>
    Stream<T>::Stream(int _fd)
        : T(_fd)
    {
        // verify the correct type
    }
}