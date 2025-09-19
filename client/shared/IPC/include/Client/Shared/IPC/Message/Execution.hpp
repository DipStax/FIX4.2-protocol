#pragma once

#include "Shared/Core/Core.hpp"
#include "Shared/Network/Buffer.hpp"

namespace ipc::msg
{
    struct Execution
    {
        std::string symbol;
        OrderId orderId;
        std::string execId;
        Price avgPrice;
        Price price;
        fix42::Side side;
        Quantity quantity;
        Quantity remainQty;

        friend net::Buffer &operator<<(net::Buffer &_buffer, const Execution &_exec);
        friend net::Buffer &operator>>(net::Buffer &_buffer, Execution &_exec);

        friend std::ostream &operator<<(std::ostream &_os, const Execution &_exec);
    };
}