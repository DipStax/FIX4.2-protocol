#pragma once

#include <chrono>
#include <memory>
#include <unordered_map>

#include "Common/Network/Socket.hpp"
#include "Server/Core/Order.hpp"

class ClientSocket
{
    public:
        ClientSocket(std::shared_ptr<net::tcp::Socket> _socket = nullptr);
        ~ClientSocket() = default;

        [[nodiscard]] std::shared_ptr<net::tcp::Socket> getSocket() const;

        bool Logged = false;
        bool Disconnect = false;
        UserId User = "";
        size_t SeqNumber = 1;

        void newRequest();
        [[nodiscard]] bool hasRequest(size_t _seqNumber) const;
        std::chrono::system_clock::time_point getRequest(size_t _seqNumber);

        bool operator==(const ClientSocket &_client) const;
        operator bool() const;

    protected:
        std::unordered_map<size_t, std::chrono::system_clock::time_point> m_request;
        std::shared_ptr<net::tcp::Socket> m_socket;
};