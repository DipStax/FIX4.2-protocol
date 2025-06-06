
#include "Common/Network/Socket.hpp"
#include "Common/Network/CSocket.hpp"

namespace net::type
{
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
    DGram<T>::DGram()
        : T(Type, 0)
    {
    }

    template<IsSocketDomain T>
    DGram<T>::DGram(int _fd)
        : T(_fd)
    {
        // verify the correct type
    }

    template<IsSocketDomain T>
    size_t DGram<T>::send(const std::string &_data)
    {
        return send(reinterpret_cast<const uint8_t *>(_data.c_str()), _data.size());
    }

    template<IsSocketDomain T>
    size_t DGram<T>::send(const uint8_t *_data, size_t _size)
    {
        return sendto(this->FD(), _data, _size, 0, reinterpret_cast<struct sockaddr *>(&this->m_addr), sizeof(this->m_addr));
    }

    template<IsSocketDomain T>
    std::string DGram<T>::receive(size_t _size, int &_error)
    {
        const uint8_t *data = c::Socket::receiveUDP(this->FD(), _size, reinterpret_cast<struct sockaddr *>(&this->m_addr), _error);
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
    bool DGram<T>::setBroadcast(bool _bc)
        requires IsINetSocketDomain<T>
    {
        m_broadcast = _bc;
        return c::Socket::setBroadcast(this->FD(), m_broadcast);
    }

    template<IsSocketDomain T>
    bool DGram<T>::isBroadcast() const
        requires IsINetSocketDomain<T>
    {
        return m_broadcast;
    }
}