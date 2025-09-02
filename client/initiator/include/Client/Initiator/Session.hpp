#pragma once

#include <memory>

#include "Client/Shared/IPC/Header.hpp"

#include "Shared/Network/Socket.hpp"
#include "Shared/Network/Buffer.hpp"
#include "Shared/Log/ILogger.hpp"

class Session
{
    public:
        enum Side
        {
            Front,
            Back
        };

        Session(const std::shared_ptr<net::INetTcp> &_front);
        ~Session() = default;

        void received(const ipc::Header &_header, net::Buffer &_buffer, Side _side);

        static std::string GetSessionId();

        std::shared_ptr<net::INetTcp> getFrontSocket() const;

    private:
        struct SessionFrontend
        {
            std::string apikey{};
            bool apikey_set = false;

            std::shared_ptr<net::INetTcp> socket = nullptr;
        };

        void handleFrontend(const ipc::Header &_header, net::Buffer &_buffer);
        void IdentifyFrontend(const ipc::Header &_header, net::Buffer &_buffer);

        const std::string m_session_id;

        SessionFrontend m_frontend{};

        std::unique_ptr<logger::ILogger> Logger = nullptr;
};