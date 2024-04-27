#include "Common/Core/Order.hpp"

std::ostream &operator<<(std::ostream &_os, const Order &_order)
{
    _os << "{ Order-id: " << _order.orderId << ", Quantity: " << _order.quantity << ", User-id: " << _order.userId << " }";
    return _os;
}

std::istream &operator>>(std::istream &_is, OrderType &_type)
{
    uint8_t type = 0;

    _is >> type;
    _type = static_cast<OrderType>(type);
    return _is;
}

std::ostream &operator<<(std::ostream &_os, OrderType _type)
{
    switch (_type) {
        case OrderType::Ask: return _os << "ask";
        case OrderType::Bid: return _os << "bid";
        default: return _os << "unknow";
    }
}

std::ostream &operator<<(std::ostream &_os, const OrderClient &_order)
{
    return _os << "{ symbol: " << _order.symbol << ", id: " << _order.orderId << ", quantity: " << _order.quantity << ", status: " << _order.status << ", type: " << _order.type << " }";
}