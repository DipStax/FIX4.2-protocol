#pragma once

#include <memory>
#include <optional>

#include "Client/Initiator/Shell/Builder.hpp"

#include "Client/Shared/IPC/Header.hpp"
#include "Client/Shared/IPC/Message/Authentication.hpp"

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

        void setBackendSocket(const std::shared_ptr<net::UnixStream> &_socket);

        [[nodiscard]] static std::string GetSessionId();

        [[nodiscard]] std::shared_ptr<net::INetTcp> getFrontSocket() const;
        [[nodiscard]] std::shared_ptr<net::UnixStream> getBackSocket() const;

        [[nodiscard]] const std::string &getApiKey() const;

        [[nodiscard]] static std::string GenerateToken();

    private:

        struct SessionFrontend
        {
            std::optional<std::string> apikey = std::nullopt;

            std::shared_ptr<net::INetTcp> socket = nullptr;
        };

        struct SessionBackend
        {
            std::optional<std::string> token = std::nullopt;

            std::unique_ptr<shell::Command> cmd = nullptr;
            std::shared_ptr<net::UnixStream> socket = nullptr;
        };

        void handleFrontend(const ipc::Header &_header, net::Buffer &_buffer);
        void identifyFrontend(net::Buffer &_buffer);
        [[nodiscard]] std::optional<std::string> login(const std::string &_apikey, const std::string &_name);
        void buildShellBack();

        void handleBackend(const ipc::Header &_header, net::Buffer &_buffer);
        void identifyBackend(net::Buffer &_buffer);

        const std::string m_session_id;

        SessionFrontend m_frontend{};
        SessionBackend m_backend{};

        std::unique_ptr<logger::ILogger> Logger = nullptr;
};