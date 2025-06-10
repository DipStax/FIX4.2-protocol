#pragma once

#include <vector>
#include <string>
#include <concepts>
#include <cstring>

#include "Common/meta.hpp"

namespace net
{
    class Buffer
    {
        public:
            Buffer() = default;
            ~Buffer() = default;

            template<IsSigned T>
            Buffer &operator<<(const T _data);
            template<IsUnsigned T>
            Buffer &operator<<(const T _data);
            template<IsFPN T>
            Buffer &operator<<(const T _data);
            Buffer &operator<<(const char *_data);
            Buffer &operator<<(const std::string &_data);

            template<IsSigned T>
            Buffer &operator>>(T &_data);
            template<IsUnsigned T>
            Buffer &operator>>(T &_data);
            template<IsFPN T>
            Buffer &operator>>(T &_data);
            Buffer& operator>>(std::string &_data);

            const std::byte *data() const;
            size_t size() const noexcept;

            void append(const void *_data, size_t _size);

        private:
            bool checkSize(std::size_t size);

            template<IsUnsigned T>
            static T convert(const T &_value);

            bool m_valid = true;
            size_t m_read = 0;

            std::vector<std::byte> m_data;
    };
}

#include "Common/Network/Buffer.inl"