#include "Client/Common/IPC/Header.hpp"
#include "Client/Common/IPC/Helper.hpp"

namespace ipc
{
    net::Buffer Helper::Status(PUStatus _status)
    {
        net::Buffer buffer;
        ipc::Header header{
            ipc::MessageType::Status,
            sizeof(PUStatus)
        };

        return buffer << header << static_cast<uint8_t>(_status);
    }

    net::Buffer Helper::Logon(const msg::Logon &_logon)
    {
        net::Buffer buffer;
        ipc::Header header{
            ipc::MessageType::Logon,
            static_cast<uint32_t>(sizeof(uint32_t) * 2 + _logon.UserId.size() + sizeof(float))
        };

        buffer << header << _logon;
        return buffer;
    }

    net::Buffer Helper::OrderSingle(const msg::OrderSingle &_order)
    {
        net::Buffer buffer;
        ipc::Header header{
            ipc::MessageType::OrderSingle,
            static_cast<uint32_t>(sizeof(uint32_t) * 2 + _order.symbol.size() + _order.orderId.size() + sizeof(Price) + sizeof(Quantity) + sizeof(uint8_t))
        };

        buffer << header << _order;
        return buffer;
    }

    net::Buffer Helper::ExecutionEvent(const msg::Execution &_exec, ipc::MessageType _type)
    {
        net::Buffer buffer;
        ipc::Header header{
            _type,
            static_cast<uint32_t>(sizeof(uint32_t) * 3 + _exec.symbol.size() + _exec.orderId.size() + _exec.execId.size() + sizeof(Price) + sizeof(Side) + sizeof(Quantity) * 2)
        };

        buffer << header << _exec;
        return buffer;
    }

}