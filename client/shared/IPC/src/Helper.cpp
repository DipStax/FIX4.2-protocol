#include "Client/Shared/IPC/Header.hpp"
#include "Client/Shared/IPC/Helper.hpp"

namespace ipc
{
    net::Buffer Helper::Auth::FrontToInitiator(const msg::AuthFrontToInitiator &_auth)
    {
        net::Buffer buffer{};
        ipc::Header header{
            ipc::MessageType::FrontToInitiatorAuth,
            static_cast<uint32_t>(sizeof(uint32_t) * 2 + _auth.apikey.size() + _auth.name.size())
        };

        buffer << header << _auth;
        return buffer;
    }

    net::Buffer Helper::Auth::InitiatorToFront(const msg::AuthInitiatorToFront &_auth)
    {
        net::Buffer buffer{};
        ipc::Header header{
            ipc::MessageType::InitiatorToFrontAuth,
            static_cast<uint32_t>(sizeof(uint32_t) + _auth.name.size())
        };

        buffer << header << _auth;
        return buffer;
    }

    net::Buffer Helper::Auth::BackToInitiator(const msg::AuthBackToInitiator &_auth)
    {
        net::Buffer buffer{};
        ipc::Header header{
            ipc::MessageType::BackToInitiatorAuth,
            static_cast<uint32_t>(sizeof(uint32_t) * 2 + _auth.apikey.size())
        };

        buffer << header << _auth;
        return buffer;
    }

    net::Buffer Helper::Auth::InitiatorToBack(const msg::AuthInitiatorToBack &_auth)
    {
        net::Buffer buffer{};
        ipc::Header header{
            ipc::MessageType::InitiatorToBackAuth,
            static_cast<uint32_t>(sizeof(uint32_t) * 2 + _auth.apikey.size() + _auth.token.size())
        };

        buffer << header << _auth;
        return buffer;
    }

    net::Buffer Helper::Auth::HandShake()
    {
        net::Buffer buffer{};
        ipc::Header header{
            ipc::MessageType::HandShakeAuth,
            0
        };

        buffer << header;
        return buffer;
    }


    net::Buffer Helper::ValidationToken::InitiatorToFront(const msg::InitiatorToFrontValidToken &_validation)
    {
        net::Buffer buffer{};
        ipc::Header header{
            ipc::MessageType::InitiatorToFrontValidToken,
            static_cast<uint32_t>(sizeof(uint32_t) * 2 + _validation.token.size())
        };

        buffer << header << _validation;
        return buffer;
    }

    net::Buffer Helper::ValidationToken::FrontToBack(const msg::FrontToBackValidToken &_validation)
    {
        net::Buffer buffer{};
        ipc::Header header{
            ipc::MessageType::FrontToBackValidToken,
            static_cast<uint32_t>(sizeof(uint32_t) + _validation.token.size())
        };

        buffer << header << _validation;
        return buffer;
    }

    net::Buffer Helper::ValidationToken::BackToFront(const msg::BackToFrontValidToken &_validation)
    {
        net::Buffer buffer{};
        ipc::Header header{
            ipc::MessageType::BackToFrontValidToken,
            static_cast<uint32_t>(sizeof(uint32_t) + _validation.token.size())
        };

        buffer << header << _validation;
        return buffer;
    }

    net::Buffer Helper::Reject(const msg::Reject &_reject)
    {
        net::Buffer buffer{};
        ipc::Header header{
            ipc::MessageType::Reject,
            static_cast<uint32_t>(sizeof(uint32_t) + _reject.message.size())
        };

        return buffer << header << _reject;
    }

    net::Buffer Helper::Status(PUStatus _status)
    {
        net::Buffer buffer{};
        ipc::Header header{
            ipc::MessageType::Status,
            sizeof(PUStatus)
        };

        return buffer << header << static_cast<uint8_t>(_status);
    }

    net::Buffer Helper::Logon(const msg::Logon &_logon)
    {
        net::Buffer buffer{};
        ipc::Header header{
            ipc::MessageType::Logon,
            static_cast<uint32_t>(sizeof(uint32_t) * 2 + _logon.UserId.size() + sizeof(float))
        };

        buffer << header << _logon;
        return buffer;
    }

    net::Buffer Helper::OrderSingle(const msg::OrderSingle &_order)
    {
        net::Buffer buffer{};
        ipc::Header header{
            ipc::MessageType::OrderSingle,
            static_cast<uint32_t>(sizeof(uint32_t) * 2 + _order.symbol.size() + _order.orderId.size()
                + sizeof(Price) + sizeof(Quantity) + sizeof(uint8_t))
        };

        buffer << header << _order;
        return buffer;
    }

    net::Buffer Helper::ExecutionNew(const msg::ExecutionNew &_exec)
    {
        net::Buffer buffer{};
        ipc::Header header{
            ipc::MessageType::ExecNew,
            static_cast<uint32_t>(sizeof(uint32_t) * 3 + _exec.symbol.size() + _exec.orderId.size() + _exec.execId.size()
                + sizeof(fix42::Side) + sizeof(fix42::OrderType) + sizeof(Price))
        };

        buffer << header << _exec;
        return buffer;
    }

    net::Buffer Helper::ExecutionEvent(const msg::ExecutionEvent &_exec)
    {
        net::Buffer buffer{};
        ipc::Header header{
            ipc::MessageType::ExecEvent,
            static_cast<uint32_t>(sizeof(uint32_t) * 3 + _exec.symbol.size() + _exec.orderId.size() + _exec.execId.size()
                + sizeof(fix42::Side) + sizeof(fix42::ExecutionStatus) + sizeof(Quantity) * 3 + sizeof(Price) * 2)
        };

        buffer << header << _exec;
        return buffer;
    }
}