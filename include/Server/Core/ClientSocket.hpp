#pragma once

#include <chrono>
#include <memory>
#include <unordered_map>
#include <set>

#include "Common/Core/Order.hpp"
#include "Common/Network/Socket.hpp"
#include "Server/Core/OrderBook.hpp"

class ClientSocket
{
    public:
        using Subs = std::vector<OrderBook::Subscription>;

        ClientSocket(std::shared_ptr<net::tcp::Socket> _socket = nullptr);
        ClientSocket(const ClientSocket &_client);
        ClientSocket(const ClientSocket &&_client) noexcept;
        ~ClientSocket() = default;

        [[nodiscard]] std::shared_ptr<net::tcp::Socket> getSocket() const;

        bool Logged = false;
        bool Disconnect = false;
        UserId User = "";
        size_t SeqNumber = 1;

        void newRequest();
        [[nodiscard]] bool hasRequest(size_t _seqNumber) const;
        std::chrono::system_clock::time_point getRequest(size_t _seqNumber);

        [[nodiscard]] ClientSocket::Subs &subscribe(const std::string &_symbol);
        void unsubscribe(const std::string &_symbol);

        ClientSocket &operator=(ClientSocket &&_client) noexcept;

        bool operator==(const ClientSocket &_client) const;
        operator bool() const;

    protected:
        using SubcribeMap = std::unordered_map<std::string, ClientSocket::Subs>;

        std::unordered_map<size_t, std::chrono::system_clock::time_point> m_request;
        std::shared_ptr<net::tcp::Socket> m_socket;
        SubcribeMap m_subscribe;
};