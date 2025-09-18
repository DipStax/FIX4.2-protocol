#pragma once

#include <chrono>
#include <memory>
#include <unordered_map>
#include <set>

#include "Shared/Core/Core.hpp"
#include "Shared/Network/Socket.hpp"
#include "Shared/Log/ILogger.hpp"

class InternalClient
{
    public:
        struct HeartBeatInfo
        {
            std::shared_ptr<InternalClient> Client = nullptr;
            std::chrono::system_clock::time_point Since{};
            float Elapsing = 1.f;
            bool TestRequest = false;
            std::optional<std::string> TestValue = std::nullopt;
        };

        InternalClient(std::shared_ptr<net::INetTcp> _socket = nullptr);
        ~InternalClient() = default;

        void login(const std::string &_user_id);
        [[nodiscard]] bool isLoggedin() const;
        [[nodiscard]] std::string getUserId() const;

        [[nodiscard]] bool isConnected() const;
        [[nodiscard]] bool close();

        void disconnect();
        void shouldDisconnect(bool _disconnect);
        [[nodiscard]] bool shouldDisconnect() const;

        void setSeqNumber(uint32_t _seqnum);
        void nextSeqNumber();
        [[nodiscard]] uint32_t getSeqNumber() const;

        [[nodiscard]] bool send(const std::byte *_data, size_t _len);
        [[nodiscard]] std::shared_ptr<net::INetTcp> getSocket() const;

        [[nodiscard]] HeartBeatInfo &getHeartBeatInfo();

        bool operator==(const InternalClient &_client) const;

        friend std::ostream &operator<<(std::ostream &_os, const InternalClient &_client);

    private:
        bool m_logged_in = false;
        bool m_should_dc = false;
        UserId m_user_id = "";
        uint32_t m_seq_num = 0;

        std::mutex m_socket_mutex;
        std::shared_ptr<net::INetTcp> m_socket = nullptr;

        HeartBeatInfo m_hb_info{};
};