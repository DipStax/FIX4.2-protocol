#pragma once

#include "Shared/Network/Buffer.hpp"

#include <string>

namespace ipc::msg
{
    struct AuthFrontToInitiator
    {
        std::string apikey{};

        friend net::Buffer &operator<<(net::Buffer &_buffer, const AuthFrontToInitiator &_auth);
        friend net::Buffer &operator>>(net::Buffer &_buffer, AuthFrontToInitiator &_auth);

        friend std::ostream &operator<<(std::ostream &_os, const AuthFrontToInitiator &_auth);
    };

    using AuthInitiatorToFront = AuthFrontToInitiator;

    struct AuthFrontToBack
    {
        std::string token{};

        friend net::Buffer &operator<<(net::Buffer &_buffer, const AuthFrontToBack &_auth);
        friend net::Buffer &operator>>(net::Buffer &_buffer, AuthFrontToBack &_auth);

        friend std::ostream &operator<<(std::ostream &_os, const AuthFrontToBack &_auth);
    };

    using AuthBackToFront = AuthFrontToBack;

    struct AuthInitiatorToBack
    {
        std::string token{};
        std::string apikey{};

        friend net::Buffer &operator<<(net::Buffer &_buffer, const AuthInitiatorToBack &_auth);
        friend net::Buffer &operator>>(net::Buffer &_buffer, AuthInitiatorToBack &_auth);

        friend std::ostream &operator<<(std::ostream &_os, const AuthInitiatorToBack &_auth);
    };

    using AuthBackToInitiator = AuthFrontToInitiator;
    // using AuthInitiatorToFront = AuthFrontToInitiator;
}