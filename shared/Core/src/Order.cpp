#include "Shared/Core/Order.hpp"

std::ostream &operator<<(std::ostream &_os, const Order &_order)
{
    _os << "{ Order-id: " << _order.orderId << ", Quantity: " << _order.quantity << ", User-id: " << _order.userId << " }";
    return _os;
}