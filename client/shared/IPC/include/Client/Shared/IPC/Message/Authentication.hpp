#pragma once

#include "Shared/Network/Buffer.hpp"

#include <string>

namespace ipc::msg
{
    struct AuthFrontToInitiator
    {
        std::string apikey{};
        std::string name{};

        friend net::Buffer &operator<<(net::Buffer &_buffer, const AuthFrontToInitiator &_auth);
        friend net::Buffer &operator>>(net::Buffer &_buffer, AuthFrontToInitiator &_auth);

        friend std::ostream &operator<<(std::ostream &_os, const AuthFrontToInitiator &_auth);
    };

    struct AuthInitiatorToFront
    {
        std::string name;

        friend net::Buffer &operator<<(net::Buffer &_buffer, const AuthInitiatorToFront &_auth);
        friend net::Buffer &operator>>(net::Buffer &_buffer, AuthInitiatorToFront &_auth);

        friend std::ostream &operator<<(std::ostream &_os, const AuthInitiatorToFront &_auth);
    };

    struct AuthInitiatorToBack
    {
        std::string token;
        std::string apikey;

        friend net::Buffer &operator<<(net::Buffer &_buffer, const AuthInitiatorToBack &_auth);
        friend net::Buffer &operator>>(net::Buffer &_buffer, AuthInitiatorToBack &_auth);

        friend std::ostream &operator<<(std::ostream &_os, const AuthInitiatorToBack &_auth);
    };

    struct AuthBackToInitiator
    {
        std::string apikey;
        uint32_t port;

        friend net::Buffer &operator<<(net::Buffer &_buffer, const AuthBackToInitiator &_auth);
        friend net::Buffer &operator>>(net::Buffer &_buffer, AuthBackToInitiator &_auth);

        friend std::ostream &operator<<(std::ostream &_os, const AuthBackToInitiator &_auth);
    };
}