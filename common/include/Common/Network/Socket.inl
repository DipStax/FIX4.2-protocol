
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
        return send(reinterpret_cast<const std::byte *>(_data.c_str()), _data.size());
    }

    template<IsSocketDomain T>
    size_t Stream<T>::send(const std::byte *_data, size_t _size)
    {
        return c::Socket::send(this->FD(), _data, _size);
    }

    template<IsSocketDomain T>
    std::vector<std::byte> Stream<T>::receive(size_t _size, int &_error)
    {
        const std::byte *data = c::Socket::receive(this->FD(), _size, _error);
        std::vector<std::byte> buffer{};

        if (_error != -1)
            buffer.assign(data, data + _error);
        if (data != nullptr)
            delete[] data;
        return buffer;
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
        return send(reinterpret_cast<const std::byte *>(_data.c_str()), _data.size());
    }

    template<IsSocketDomain T>
    size_t DGram<T>::send(const std::byte *_data, size_t _size)
    {
        return c::Socket::sendTo(this->FD(), _data, _size, reinterpret_cast<struct sockaddr *>(&this->m_addr), sizeof(this->m_addr));
    }

    template<IsSocketDomain T>
    std::vector<std::byte> DGram<T>::receive(size_t _size, int &_error)
    {
        const std::byte *data = c::Socket::receiveFrom(this->FD(), _size, reinterpret_cast<struct sockaddr *>(&this->m_addr), _error);
        std::vector<std::byte> buffer{};

        if (_error != -1)
            buffer.assign(data, data + _error);
        if (data != nullptr)
            delete[] data;
        return buffer;
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