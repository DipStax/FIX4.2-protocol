#pragma once

#include "Common/Core/Order.hpp"
#include "Common/Network/Buffer.hpp"

namespace ipc::msg
{
    struct Execution
    {
        std::string symbol;
        std::string orderId;
        Price price;
        Quantity remainQty;

        friend net::Buffer &operator<<(net::Buffer &_buffer, const Execution &_exec);
        friend net::Buffer &operator>>(net::Buffer &_buffer, Execution &_exec);

        friend std::ostream &operator<<(std::ostream &_os, const Execution &_exec);
    };
}