#include "Client/Common/IPC/Message/OrderSingle.hpp"

namespace ipc::msg
{
    net::Buffer &operator<<(net::Buffer &_buffer, const OrderSingle &_order)
    {
        return _buffer << _order.symbol << _order.orderId << _order.price << _order.quantity << static_cast<uint8_t>(_order.type);
    }

    net::Buffer &operator>>(net::Buffer &_buffer, OrderSingle &_order)
    {
        uint8_t type;

        _buffer >> _order.symbol >> _order.orderId >> _order.price >> _order.quantity >> type;
        _order.type = static_cast<OrderType>(type);
        return _buffer;
    }

    std::ostream &operator<<(std::ostream &_os, const OrderSingle &_order)
    {
        return _os << "{ Symbol: " << _order.symbol << ", OrderId: " << _order.orderId << ", Price: " << _order.price
            << ", Quantity: " << _order.quantity << ", Type: " << static_cast<int>(_order.type) << " }";
    }
}