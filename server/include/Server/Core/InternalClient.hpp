#pragma once

#include <chrono>
#include <memory>
#include <unordered_map>
#include <set>

#include "Server/Core/OrderBook.hpp"

#include "Common/Core/Order.hpp"
#include "Common/Network/Socket.hpp"

class InternalClient
{
    public:
        using Subs = std::vector<OrderBook::Subscription>;

        struct HeartBeatInfo
        {
            std::shared_ptr<InternalClient> Client = nullptr;
            std::chrono::system_clock::time_point Since{};
            bool TestRequest = false;
            std::optional<std::string> TestValue = std::nullopt;
        };

        InternalClient(std::shared_ptr<net::tcp::Socket> _socket = nullptr);
        ~InternalClient() = default;

        void login(const std::string &_user_id);
        [[nodiscard]] bool isLoggedin() const;
        [[nodiscard]] std::string getUserId() const;

        void disconnect();
        void shouldDisconnect(bool _disconnect);
        [[nodiscard]] bool shouldDisconnect() const;

        void setSeqNumber(size_t _seqnum);
        size_t nextSeqNumber();
        [[nodiscard]] size_t getSeqNumber() const;

        [[nodiscard]] std::shared_ptr<net::tcp::Socket> getSocket() const;

        [[nodiscard]] HeartBeatInfo &getHeartBeatInfo();

        [[nodiscard]] bool isSubscribeTo(const std::string &_symbol);
        [[nodiscard]] Subs &subscribe(const std::string &_symbol);
        void unsubscribe(const std::string &_symbol);

        bool operator==(const InternalClient &_client) const;

        friend std::ostream &operator<<(std::ostream &_os, const InternalClient &_client);

    private:
        bool m_logged_in = false;
        bool m_should_dc = false;
        UserId m_user_id = "";
        size_t m_seq_num = 1;

        using SubcribeMap = std::unordered_map<std::string, InternalClient::Subs>;

        std::shared_ptr<net::tcp::Socket> m_socket = nullptr;
        std::unordered_map<size_t, std::chrono::system_clock::time_point> m_request{};
        SubcribeMap m_subscribe{};
        HeartBeatInfo m_hb_info{};
};