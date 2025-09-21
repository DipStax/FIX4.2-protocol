#pragma once

#include "Shared/Network/Buffer.hpp"

namespace ipc::msg
{
    struct Reject
    {
        std::string message;

        friend net::Buffer &operator<<(net::Buffer &_buffer, const Reject &_order);
        friend net::Buffer &operator>>(net::Buffer &_buffer, Reject &_order);

        friend std::ostream &operator<<(std::ostream &_os, const Reject &_order);
    };
}