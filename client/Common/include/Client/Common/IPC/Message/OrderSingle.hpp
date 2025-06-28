#pragma once

#include "Common/Network/Buffer.hpp"
#include "Common/Core/Order.hpp"

namespace ipc::msg
{
    struct OrderSingle
    {
        std::string symbol;
        std::string orderId;
        Price price;
        Quantity quantity;
        OrderType type;

        friend net::Buffer &operator<<(net::Buffer &_buffer, const OrderSingle &_order);
        friend net::Buffer &operator>>(net::Buffer &_buffer, OrderSingle &_order);

        friend std::ostream &operator<<(std::ostream &_os, const OrderSingle &_order);
    };
}