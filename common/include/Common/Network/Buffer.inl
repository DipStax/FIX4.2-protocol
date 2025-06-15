#include <arpa/inet.h>

#include "Common/Network/Buffer.hpp"

namespace net
{
    template<IsSigned T>
    Buffer &Buffer::operator<<(const T _data)
    {
        using UnsignedT = typename std::make_unsigned<T>::type;

        const T write = static_cast<T>(convert<UnsignedT>(static_cast<UnsignedT>(_data)));

        append(&write, sizeof(T));
        return *this;
    }

    template<IsUnsigned T>
    Buffer &Buffer::operator<<(const T _data)
    {
        const T write = convert<T>(_data);

        append(&write, sizeof(T));
        return *this;
    }

    template<IsFPN T>
    Buffer &Buffer::operator<<(const T _data)
    {
        append(&_data, sizeof(T));
        return *this;
    }

    template<IsSigned T>
    Buffer &Buffer::operator>>(T &_data)
    {
        using UnsignedT = typename std::make_unsigned<T>::type;

        if (checkSize(sizeof(T))) {
            std::memcpy(&_data, &m_data[m_read], sizeof(T));
            _data = static_cast<T>(convert<UnsignedT>(static_cast<UnsignedT>(_data)));
            m_read += sizeof(T);
        }
        return *this;
    }

    template<IsUnsigned T>
    Buffer &Buffer::operator>>(T &_data)
    {
        if (checkSize(sizeof(T))) {
            std::memcpy(&_data, &m_data[m_read], sizeof(T));
            _data = convert<T>(_data);
            m_read += sizeof(T);
        }
        return *this;
    }

    template<IsFPN T>
    Buffer &Buffer::operator>>(T &_data)
    {
        if (checkSize(sizeof(_data)))
        {
            std::memcpy(&_data, &m_data[m_read], sizeof(T));
            m_read += sizeof(T);
        }
        return *this;
    }

    template<IsUnsigned T>
    T Buffer::convert(const T &_value)
    {
        if constexpr (sizeof(T) == 1)
            return _value;
        else if constexpr (sizeof(T) == 2)
            return htons(_value);
        else if constexpr (sizeof(T) == 4)
            return htonl(_value);
        else
            static_assert(false, "type not supported");
    }
}
