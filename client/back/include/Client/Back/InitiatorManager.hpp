#pragma once

#include <thread>
#include <cstdlib>
#include <memory>

#include "Shared/Network/Socket.hpp"
#include "Shared/Log/ILogger.hpp"
#include "Client/Shared/IPC/IPCNetworkManager.hpp"

class InitiatorManager : public IPCNetworkManager<net::UnixStream>
{
    public:
        static InitiatorManager &Instance();

        ~InitiatorManager() = default;

        void connect();

    private:
        InitiatorManager();

        void onError(int _errno) final;
        void onDisconnection() final;
        void onWrongSize(const std::vector<std::byte> &_byte, int _readsize) final;
        void onWrongBodySize(const std::vector<std::byte> &_byte, int _readsize) final;

        void onReceive(net::Buffer &_buffer) final;

    private:
        std::jthread m_thread{};
};