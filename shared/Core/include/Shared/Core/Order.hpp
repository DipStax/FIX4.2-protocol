#pragma once

#include <cstdint>
#include <iostream>
#include <list>

using UserId = std::string;
using OrderId = std::string;
using Quantity = uint32_t;
using Side = uint8_t;

struct Order
{
    UserId userId;
    OrderId orderId;
    Quantity quantity;
};

std::ostream &operator<<(std::ostream &_os, const Order &_order);

using Price = double;
using OrderList = std::list<Order>;

/// @brief Available value for OrdStatus [39](https://www.onixs.biz/fix-dictionary/4.2/tagNum_39.html).
enum OrderStatusValue
{
    New = '0',
    PartiallyFilled,
    Filled,
    Canceld = '4',
    Replaced,
    Pending,
    Rejected = '8'
};
using ExecTypeValue = OrderStatusValue;

enum class OrderType : uint8_t
{
    Ask,
    Bid
};

std::istream &operator>>(std::istream &_is, OrderType &_type);
std::ostream &operator<<(std::ostream &_os, OrderType _type);