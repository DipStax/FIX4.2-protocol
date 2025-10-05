#pragma once

#include "Shared/Core/Core.hpp"
#include "Shared/Network/Buffer.hpp"

namespace ipc::msg
{
    struct ExecutionNew
    {
        OrderId orderId;
        std::string execId;
        std::string symbol;
        fix42::Side side;
        fix42::OrderType orderType;
        Quantity quantity;
        Price price;

        friend net::Buffer &operator<<(net::Buffer &_buffer, const ExecutionNew &_exec);
        friend net::Buffer &operator>>(net::Buffer &_buffer, ExecutionNew &_exec);

        friend std::ostream &operator<<(std::ostream &_os, const ExecutionNew &_exec);
    };

    struct ExecutionEvent
    {
        OrderId orderId;
        std::string execId;
        fix42::ExecutionStatus execStatus;
        std::string symbol;
        fix42::Side side;
        Quantity lastShare;
        Quantity leaveQty;
        Quantity cumQty;
        Price avgPrice;
        Price lastPrice;

        friend net::Buffer &operator<<(net::Buffer &_buffer, const ExecutionEvent &_exec);
        friend net::Buffer &operator>>(net::Buffer &_buffer, ExecutionEvent &_exec);

        friend std::ostream &operator<<(std::ostream &_os, const ExecutionEvent &_exec);
    };
}