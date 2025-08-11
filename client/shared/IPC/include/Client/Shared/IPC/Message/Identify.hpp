#pragma once

#include "Shared/Network/Buffer.hpp"

#include <string>

namespace ipc::msg
{
    struct IdentifyFront
    {
        std::string apiKey;

        friend net::Buffer &operator<<(net::Buffer &_buffer, const IdentifyFront &_identify);
        friend net::Buffer &operator>>(net::Buffer &_buffer, IdentifyFront &_identify);

        friend std::ostream &operator<<(std::ostream &_os, const IdentifyFront &_identify);
    };
}