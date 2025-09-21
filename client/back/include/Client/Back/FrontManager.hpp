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

        /// @brief Verify if the message contain in _buffer is related to the authentication validation and process it if so.
        /// @param _buffer Buffer containing the header and the message.
        /// @return Return `false` if the message is not related to auth validation and should processed as usual
        /// otherwise return `true` and the message has been handle.
        bool authValidation(net::Buffer &_buffer);

    private:
        FrontManager();

        bool m_auth = false;
        bool m_handshake = false;

        std::optional<std::string> m_token = std::nullopt;

        ts::Queue<net::Buffer> m_output{};
        std::jthread m_thread{};
        net::Acceptor<net::INetTcp> m_acceptor{};
};