#include "Client/Common/IPC/Message/Execution.hpp"

namespace ipc::msg
{
    net::Buffer &operator<<(net::Buffer &_buffer, const Execution &_exec)
    {
        return _buffer << _exec.symbol << _exec.orderId << _exec.price << _exec.remainQty;
    }

    net::Buffer &operator>>(net::Buffer &_buffer, Execution &_exec)
    {
        return _buffer >> _exec.symbol >> _exec.orderId >> _exec.price >> _exec.remainQty;
    }

    std::ostream &operator<<(std::ostream &_os, const Execution &_exec)
    {
        return _os << "{ Symbol: " << _exec.symbol << ", OrderId: " << _exec.orderId << ", Price: " << _exec.price
            << ", Quantity: " << _exec.remainQty << " }";
    }
}