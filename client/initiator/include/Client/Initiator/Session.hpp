#pragma once

#include <memory>

#include "Client/Initiator/Shell/Builder.hpp"

#include "Client/Shared/IPC/Header.hpp"
#include "Client/Shared/IPC/Message/Identify.hpp"

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

        void send(const net::Buffer &_buffer, Side _side);

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

        struct SessionBackend
        {
            std::unique_ptr<shell::Command> cmd = nullptr;
            std::shared_ptr<net::UnixStream> socket = nullptr;
        };

        void handleFrontend(const ipc::Header &_header, net::Buffer &_buffer);
        void identifyFrontend(const ipc::Header &_header, net::Buffer &_buffer);

        void setupBackend(const ipc::msg::AuthFrontToInitiator &_identify);

        const std::string m_session_id;

        SessionFrontend m_frontend{};
        SessionBackend m_backend{};

        std::unique_ptr<logger::ILogger> Logger = nullptr;
};