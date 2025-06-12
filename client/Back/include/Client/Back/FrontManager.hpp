#pragma once

#include "Common/Network/Buffer.hpp"
#include "Common/Network/Socket.hpp"
#include "Common/Network/Acceptor.hpp"
#include "Common/Container/ProcessUnit.hpp"

#include "Common/Log/ILogger.hpp"

class FrontManager
{
    public:
        ~FrontManager() = default;

        static FrontManager &Instance();

        void notify(const net::Buffer &_buffer);

        /// @brief Wait the connection of the frontend
        void wait_frontend();

        void notify_status(PUStatus _status);

    private:
        FrontManager();

        net::Acceptor<net::UnixStream>::Client m_socket;

        std::unique_ptr<logger::ILogger> Logger = nullptr;
};