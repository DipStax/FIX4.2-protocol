#include "Shared/Network/Buffer.hpp"

namespace net
{
    Buffer &Buffer::operator<<(const char *_data)
    {
        const uint32_t length = static_cast<uint32_t>(std::strlen(_data));

        *this << length;
        append(_data, length * sizeof(char));
        return *this;
    }

    Buffer &Buffer::operator<<(const std::string &_data)
    {
        const uint32_t size = static_cast<uint32_t>(_data.size());

        *this << size;
        if (size > 0)
            append(_data.c_str(), size);
        return *this;
    }

    Buffer &Buffer::operator>>(std::string &_data)
    {
        uint32_t size = 0;

        *this >> size;
        _data.clear();
        if (size > 0 && checkSize(size)) {
            _data.assign(reinterpret_cast<const char *>(&m_data[m_read]), size);
            m_read += size;
        }
        return *this;
    }

    const std::byte *Buffer::data() const
    {
        return !m_data.empty() ? m_data.data() : nullptr;
    }

    size_t Buffer::size() const noexcept
    {
        return m_data.size();
    }

    void Buffer::reset()
    {
        m_read = 0;
    }

    void Buffer::clear()
    {
        reset();
        m_data.clear();
    }

    void Buffer::append(const void *_data, size_t _size)
    {
        if (_data && (_size > 0))
        {
            const std::byte *begin = reinterpret_cast<const std::byte *>(_data);
            const std::byte *end = begin + _size;

            m_data.insert(m_data.end(), begin, end);
        }
    }

    bool Buffer::checkSize(std::size_t size)
    {
        const bool overflow = m_read + size < m_read;

        m_valid = m_valid && m_read + size <= m_data.size() && !overflow;
        return m_valid;
    }
}