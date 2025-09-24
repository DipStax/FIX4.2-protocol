#include "Shared/Core/Core.hpp"

std::ostream &operator<<(std::ostream &_os, const Order &_order)
{
    _os << "{ Order-id: " << _order.orderId << ", User-id: " << _order.userId << ", OrgQty: " << _order.originalQty
        << ", RemQty: " << _order.remainQty << ", AvgPrice: " << _order.avgPrice << ", Side: " << _order.side
        << ", Status: " << _order.status << " }";
    return _os;
}