#include "Client/Shared/IPC/Message/Execution.hpp"

namespace ipc::msg
{
    net::Buffer &operator<<(net::Buffer &_buffer, const ExecutionNew &_exec)
    {
        return _buffer << _exec.orderId << _exec.execId << _exec.symbol << static_cast<uint8_t>(_exec.side)
            << static_cast<uint8_t>(_exec.orderType) << _exec.quantity << _exec.price;
    }

    net::Buffer &operator>>(net::Buffer &_buffer, ExecutionNew &_exec)
    {
        uint8_t side = 0;
        uint8_t orderType = 0;

        _buffer >> _exec.orderId >> _exec.execId >> _exec.symbol >> side >> orderType >> _exec.quantity >> _exec.price;

        _exec.side = static_cast<fix42::Side>(side);
        _exec.orderType = static_cast<fix42::OrderType>(orderType);
        return _buffer;
    }

    std::ostream &operator<<(std::ostream &_os, const ExecutionNew &_exec)
    {
        return _os
            << "OrderId: " << _exec.orderId
            << ", ExecType: " << _exec.execId
            << ", Symbol: " << _exec.symbol
            << ", Side: " << static_cast<uint8_t>(_exec.side)
            << ", OrderType: " << static_cast<char>(_exec.orderType)
            << ", Price: " << _exec.price;
    }

    net::Buffer &operator<<(net::Buffer &_buffer, const ExecutionEvent &_exec)
    {
        return _buffer << _exec.orderId << _exec.execId << static_cast<uint8_t>(_exec.execStatus) << _exec.symbol
            << static_cast<uint8_t>(_exec.side) << _exec.lastShare << _exec.leaveQty << _exec.cumQty
            << _exec.avgPrice << _exec.lastPrice;
    }

    net::Buffer &operator>>(net::Buffer &_buffer, ExecutionEvent &_exec)
    {
        uint8_t side = 0;
        uint8_t execType = 0;

        _buffer >> _exec.orderId >> _exec.execId >> execType >> _exec.symbol >> side
            >> _exec.lastShare >> _exec.leaveQty >> _exec.cumQty >> _exec.avgPrice >> _exec.lastPrice;

        _exec.side = static_cast<fix42::Side>(side);
        _exec.execStatus = static_cast<fix42::ExecutionStatus>(execType);
        return _buffer;
    }

    std::ostream &operator<<(std::ostream &_os, const ExecutionEvent &_exec)
    {
        return _os
            << "OrderId: " << _exec.orderId
            << ", ExecType: " << _exec.execId
            << ", ExecStatus: " << static_cast<char>(_exec.execStatus)
            << ", Symbol: " << _exec.symbol
            << ", Side: " << static_cast<uint8_t>(_exec.side)
            << ", LastShare: " << _exec.lastShare
            << ", LeaveQty: " << _exec.leaveQty
            << ", CumQty: " << _exec.cumQty
            << ", AvgPrice: " << _exec.avgPrice
            << ", LastPrice:" << _exec.lastPrice;
    }
}