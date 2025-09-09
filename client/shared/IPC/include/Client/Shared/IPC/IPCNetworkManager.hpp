#pragma once

#include <thread>
#include <vector>

#include "Shared/Network/Buffer.hpp"
#include "Shared/Log/ILogger.hpp"

template<class SocketType>
class IPCNetworkManager
{
    public:
        void send(const net::Buffer &_buffer);

    protected:
        IPCNetworkManager(const std::string &_name);

        virtual void onError(int _errno) = 0;
        virtual void onDisconnection() = 0;
        virtual void onWrongSize(const std::vector<std::byte> &_byte, int _readsize) = 0;
        virtual void onWrongBodySize(const std::vector<std::byte> &_byte, int _readsize) = 0;

        virtual void onReceive(net::Buffer &_buffer) = 0;

        void receiveLoop(std::stop_token _st);

        std::shared_ptr<SocketType> m_socket = nullptr;

        std::unique_ptr<logger::ILogger> Logger = nullptr;
};

#include "Client/Shared/IPC/IPCNetworkManager.inl"