#pragma once

#include <thread>
#include <optional>

#include "Client/Shared/IPC/IPCNetworkManager.hpp"

#include "Shared/Log/ILogger.hpp"
#include "Shared/Network/Acceptor.hpp"
#include "Shared/Network/Buffer.hpp"
#include "Shared/Thread/Queue.hpp"

class FrontManager : public IPCNetworkManager<net::INetTcp>
{
    public:
        ~FrontManager() = default;

        static FrontManager &Instance();

        void wait_frontend();

        ts::Queue<net::Buffer> &getMessageQueue();

    protected:
        void onError(int _errno) final;
        void onDisconnection() final;
        void onWrongSize(const std::vector<std::byte> &_byte, int _readsize) final;
        void onWrongBodySize(const std::vector<std::byte> &_byte, int _readsize) final;

        void onReceive(net::Buffer &_buffer) final;

        friend class InitiatorManager;

        uint32_t initAcceptor();
        void setupToken(const std::string &_token);


    private:
        FrontManager();

        bool m_auth = false;
        std::optional<std::string> m_token = std::nullopt;

        ts::Queue<net::Buffer> m_output{};
        std::jthread m_thread{};
        net::Acceptor<net::INetTcp> m_acceptor{};
};