#include "Client/Shared/IPC/Message/Execution.hpp"

namespace ipc::msg
{
    net::Buffer &operator<<(net::Buffer &_buffer, const Execution &_exec)
    {
        return _buffer << _exec.symbol << _exec.orderId << _exec.execId << _exec.avgPrice << _exec.price << _exec.side << _exec.quantity << _exec.remainQty;
    }

    net::Buffer &operator>>(net::Buffer &_buffer, Execution &_exec)
    {
        return _buffer >> _exec.symbol >> _exec.orderId >> _exec.execId >> _exec.avgPrice >> _exec.price >> _exec.side >> _exec.quantity >> _exec.remainQty;
    }

    std::ostream &operator<<(std::ostream &_os, const Execution &_exec)
    {
        return _os << "{ Symbol: " << _exec.symbol << ", ExecId: " << _exec.execId << ", OrderId: " << _exec.orderId << ", Price: " << _exec.price
            << ", Avg Price: " << _exec.avgPrice << ", Side: " << _exec.side << ", Quantity: " << _exec.remainQty << ", Remain Qty:" << _exec.remainQty << " }";
    }
}