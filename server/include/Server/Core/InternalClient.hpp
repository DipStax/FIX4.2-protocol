#pragma once

#include <chrono>
#include <memory>
#include <unordered_map>
#include <set>

#include "Server/Core/OrderBook.hpp"

#include "Shared/Core/Order.hpp"
#include "Shared/Network/Socket.hpp"

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

        void disconnect();
        void shouldDisconnect(bool _disconnect);
        [[nodiscard]] bool shouldDisconnect() const;

        void setSeqNumber(size_t _seqnum);
        void nextSeqNumber();
        [[nodiscard]] size_t getSeqNumber() const;

        [[nodiscard]] std::shared_ptr<net::INetTcp> getSocket() const;

        [[nodiscard]] HeartBeatInfo &getHeartBeatInfo();

        bool operator==(const InternalClient &_client) const;

        friend std::ostream &operator<<(std::ostream &_os, const InternalClient &_client);

    private:
        bool m_logged_in = false;
        bool m_should_dc = false;
        UserId m_user_id = "";
        size_t m_seq_num = 0;

        std::shared_ptr<net::INetTcp> m_socket = nullptr;
        std::unordered_map<size_t, std::chrono::system_clock::time_point> m_request{};
        HeartBeatInfo m_hb_info{};
};