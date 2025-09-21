#pragma once

#include "Shared/Network/Buffer.hpp"

namespace ipc::msg
{
    struct InitiatorToFrontValidToken
    {
        std::string token;
        uint32_t port;

        friend net::Buffer &operator<<(net::Buffer &_buffer, const InitiatorToFrontValidToken &_validation);
        friend net::Buffer &operator>>(net::Buffer &_buffer, InitiatorToFrontValidToken &_validation);

        friend std::ostream &operator<<(std::ostream &_os, const InitiatorToFrontValidToken &_validation);
    };

    struct FrontToBackValidToken
    {
        std::string token;

        friend net::Buffer &operator<<(net::Buffer &_buffer, const FrontToBackValidToken &_validation);
        friend net::Buffer &operator>>(net::Buffer &_buffer, FrontToBackValidToken &_validation);

        friend std::ostream &operator<<(std::ostream &_os, const FrontToBackValidToken &_validation);
    };

    using BackToFrontValidToken = FrontToBackValidToken;
}