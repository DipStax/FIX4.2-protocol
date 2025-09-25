#pragma once

#include <cstdint>
#include <iostream>
#include <list>

using UserId = std::string;
using OrderId = std::string;
using Quantity = float;
using Price = float;
using Side = uint8_t;

#include "Shared/Core/Enum.hpp"

struct Order
{
    UserId userId;
    OrderId orderId;
    Quantity originalQty;
    Quantity remainQty;
    Price avgPrice;
    fix42::Side side;
    fix42::OrderStatus status;
};

std::ostream &operator<<(std::ostream &_os, const Order &_order);
using OrderList = std::list<Order>;
