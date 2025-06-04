#pragma once

#include "Common/Network/BaseSocket.hpp"
#include "Common/Network/Socket.hpp"

namespace net
{
    template<IsSocketDomain T, bool Enable = (T::Domain == AF_UNIX)>
    class UnixPathHolder
    {
    };

    template<IsSocketDomain T>
    class UnixPathHolder<T, true>
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
                requires (T::Domain == AF_INET);

            bool listen(const std::string &_path)
                requires (T::Domain == AF_UNIX);

            Client accept();

            bool unlink()
                requires (T::Domain == AF_UNIX);
    };
}

#include "Common/Network/Acceptor.inl"
