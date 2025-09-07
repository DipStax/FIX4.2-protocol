#include "Client/Shared/IPC/Header.hpp"
#include "Client/Shared/IPC/Helper.hpp"

namespace ipc
{
    net::Buffer Helper::Auth::FrontToInitiator(const msg::AuthFrontToInitiator &_auth)
    {
        net::Buffer buffer;
        ipc::Header header{
            ipc::MessageType::FrontToInitiatorAuth,
            static_cast<uint32_t>(sizeof(uint32_t) + _auth.apikey.size())
        };

        buffer << header << _auth;
        return buffer;
    }

    net::Buffer Helper::Auth::InitiatorToFront(const msg::AuthInitiatorToFront &_auth)
    {
        net::Buffer buffer;
        ipc::Header header{
            ipc::MessageType::InitiatorToFrontAuth,
            static_cast<uint32_t>(sizeof(uint32_t) + _auth.apikey.size())
        };

        buffer << header << _auth;
        return buffer;
    }

    net::Buffer Helper::Auth::BackToInitiator(const msg::AuthBackToInitiator &_auth)
    {
        net::Buffer buffer;
        ipc::Header header{
            ipc::MessageType::BackToInitiatorAuth,
            static_cast<uint32_t>(sizeof(uint32_t) + _auth.apikey.size())
        };

        buffer << header << _auth;
        return buffer;
    }

    net::Buffer Helper::Auth::InitiatorToBack(const msg::AuthInitiatorToBack &_auth)
    {
        net::Buffer buffer;
        ipc::Header header{
            ipc::MessageType::InitiatorToBackAuth,
            static_cast<uint32_t>(sizeof(uint32_t) * 2 + _auth.apikey.size() + _auth.token.size())
        };

        buffer << header << _auth;
        return buffer;
    }

    net::Buffer Helper::Auth::BackToFront(const msg::AuthBackToFront &_auth)
    {
        net::Buffer buffer;
        ipc::Header header{
            ipc::MessageType::BackToFrontAuth,
            static_cast<uint32_t>(sizeof(uint32_t) + _auth.token.size())
        };

        buffer << header << _auth;
        return buffer;
    }


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
            static_cast<uint32_t>(sizeof(uint32_t) * 3 + _exec.symbol.size() + _exec.orderId.size() + _exec.execId.size() + sizeof(Price) * 2 + sizeof(Side) + sizeof(Quantity) * 2)
        };

        buffer << header << _exec;
        return buffer;
    }

}