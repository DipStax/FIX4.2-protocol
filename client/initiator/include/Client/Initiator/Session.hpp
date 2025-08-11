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

    private:
        void handleFrontend(const ipc::Header &_header, net::Buffer &_buffer);
        void IdentifyFrontend(const ipc::Header &_header, net::Buffer &_buffer);

        const std::string m_session_id{};
        std::string m_apikey;

        bool m_apikey_set = false;

        std::shared_ptr<net::INetTcp> m_front_socket = nullptr;

        std::unique_ptr<logger::ILogger> Logger = nullptr;
};