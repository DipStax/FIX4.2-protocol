#pragma once

#include <chrono>
#include <memory>
#include <unordered_map>
#include <set>

#include "Common/Core/Order.hpp"
#include "Common/Network/Socket.hpp"
#include "Server/Core/OrderBook.hpp"

class InternalClient
{
    public:
        using Subs = std::vector<OrderBook::Subscription>;

        InternalClient(std::shared_ptr<net::tcp::Socket> _socket = nullptr);
        InternalClient(const InternalClient &_client);
        InternalClient(const InternalClient &&_client) noexcept;
        ~InternalClient() = default;

        bool Logged = false;
        bool Disconnect = false;
        UserId User = "";
        size_t SeqNumber = 1;
        size_t ClientSeqNumber = 1;

        [[nodiscard]] std::shared_ptr<net::tcp::Socket> getSocket() const;

        void newRequest();
        [[nodiscard]] bool hasRequest(size_t _seqNumber) const;
        std::chrono::system_clock::time_point getRequest(size_t _seqNumber);

        [[nodiscard]] InternalClient::Subs &subscribe(const std::string &_symbol);
        void unsubscribe(const std::string &_symbol);

        InternalClient &operator=(InternalClient &&_client) noexcept;

        bool operator==(const InternalClient &_client) const;
        operator bool() const;

        friend std::ostream &operator<<(std::ostream &_os, const InternalClient &_client);

    protected:
        using SubcribeMap = std::unordered_map<std::string, InternalClient::Subs>;

        std::shared_ptr<net::tcp::Socket> m_socket = nullptr;
        std::unordered_map<size_t, std::chrono::system_clock::time_point> m_request{};
        SubcribeMap m_subscribe{};
};