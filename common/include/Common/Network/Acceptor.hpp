#pragma once

#include "Common/Network/Socket.hpp"

namespace net
{
    template<class T>
    class UnixPathHolder
    {
        static_assert(false, "UnixPathHolder<T>: unsupported socket domain");
    };

    template<IsINetSocketDomain T>
    class UnixPathHolder<T>
    {
    };

    template<IsUnixSocketDomain T>
    class UnixPathHolder<T>
    {
        protected:
            std::string m_path = "";
    };

    template<IsSocketDomain T>
    class Acceptor : public BaseSocket, UnixPathHolder<T>
    {
        public:
            using Client = std::shared_ptr<T>;

            Acceptor();
            ~Acceptor();

            bool listen(uint32_t _port)
                requires IsINetSocketDomain<T>;

            bool listen(const std::string &_path)
                requires IsUnixSocketDomain<T>;

            Client accept();

            bool unlink()
                requires IsUnixSocketDomain<T>;
    };
}

#include "Common/Network/Acceptor.inl"
