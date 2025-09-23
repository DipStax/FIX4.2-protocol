#include "Shared/Core/Core.hpp"

std::ostream &operator<<(std::ostream &_os, const Order &_order)
{
    _os << "{ Order-id: " << _order.orderId << ", Quantity: " << _order.originalQty << ", User-id: " << _order.userId << " }";
    return _os;
}