#pragma once

#include "Client/Shared/IPC/Message/Message.hpp"
#include "Client/Shared/IPC/MessageType.hpp"

#include "Shared/Network/Buffer.hpp"
#include "Shared/ProcessUnit/ProcessUnit.hpp"

namespace ipc
{
    struct Helper
    {
        struct Auth
        {
            static net::Buffer FrontToInitiator(const msg::AuthFrontToInitiator &_auth);
            static net::Buffer InitiatorToFront(const msg::AuthInitiatorToFront &_auth);
            static net::Buffer BackToInitiator(const msg::AuthBackToInitiator &_auth);
            static net::Buffer InitiatorToBack(const msg::AuthInitiatorToBack &_auth);
        };

        struct ValidationToken
        {
            static net::Buffer InitiatorToFront(const msg::InitiatorToFrontValidToken &_validation);
            static net::Buffer FrontToBack(const msg::FrontToBackValidToken &_validation);
            static net::Buffer BackToFront(const msg::BackToFrontValidToken &_validation);
        };

        static net::Buffer Reject(const msg::Reject &_reject);
        static net::Buffer Status(PUStatus _status);
        static net::Buffer Logon(const msg::Logon &_logon);
        static net::Buffer OrderSingle(const msg::OrderSingle &_order);
        static net::Buffer ExecutionEvent(const msg::Execution &_exec, MessageType _type);
    };
}