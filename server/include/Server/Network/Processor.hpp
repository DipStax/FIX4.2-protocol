#pragma once

#include <memory>

#include "Common/Core/meta.hpp"
#include "Common/Network/Socket.hpp"
#include "Server/Core/Pipeline/Naming.hpp"

namespace net::tcp
{
    namespace in
    {
        class Basic
        {
            public:
                static bool run(const ClientStore::Client &_client, InputRouter &_serial, InOutNetwork &_error);
        };
    }

    namespace out
    {
        class Response
        {
            public:
                static bool run(Context<OutNetworkInput> &_input);
        };
    }
}