#pragma once

#include <cstdint>
#include <iostream>
#include <list>

using UserId = std::string;
using OrderId = std::string;
using Quantity = float;
using Price = float;
using Side = uint8_t;

struct Order
{
    UserId userId;
    OrderId orderId;
    Quantity quantity;
};

std::ostream &operator<<(std::ostream &_os, const Order &_order);

using OrderList = std::list<Order>;

#include "Shared/Core/Enum.hpp"