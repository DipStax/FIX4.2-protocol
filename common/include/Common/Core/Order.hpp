#pragma once

#include <cstdint>
#include <iostream>
#include <vector>

using UserId = std::string;
using OrderId = std::string;
using Quantity = uint64_t;
using Side = uint8_t;

struct Order
{
    UserId userId;
    OrderId orderId;
    Quantity quantity;
};

std::ostream &operator<<(std::ostream &_os, const Order &_order);

using Price = double;
using OrderList = std::vector<Order>;

/// @brief Available value for OrdStatus [39](https://www.onixs.biz/fix-dictionary/4.2/tagNum_39.html).
enum OrderStatus {
    New = 0,
    PartiallyFilled,
    Filled,
    Canceld = 4,
    Replaced,
    Pending,
    Rejected = 8
};

enum class OrderType : uint8_t
{
    Ask,
    Bid
};

std::istream &operator>>(std::istream &_is, OrderType &_type);
std::ostream &operator<<(std::ostream &_os, OrderType _type);

struct OrderClient
{
    OrderId orderId;
    Quantity quantity;
    OrderStatus status;
    OrderType type;
    Price price;
    std::string symbol;
};

std::ostream &operator<<(std::ostream &_os, const OrderClient &_order);