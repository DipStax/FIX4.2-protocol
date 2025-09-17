#pragma once

#include "Shared/Network/Buffer.hpp"
#include "Shared/Core/Order.hpp"
#include "Shared/Core/Enum.hpp"

namespace ipc::msg
{
    struct OrderSingle
    {
        std::string symbol;
        std::string orderId;
        Price price;
        Quantity quantity;
        fix42::Side type;

        friend net::Buffer &operator<<(net::Buffer &_buffer, const OrderSingle &_order);
        friend net::Buffer &operator>>(net::Buffer &_buffer, OrderSingle &_order);

        friend std::ostream &operator<<(std::ostream &_os, const OrderSingle &_order);
    };
}