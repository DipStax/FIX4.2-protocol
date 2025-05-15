#pragma once

#include <memory>

#include "Common/Core/meta.hpp"
#include "Common/Network/Socket.hpp"
#include "Server/Core/ProcessUnit/data/Global.hpp"

namespace net::tcp
{
    namespace in
    {
        class Basic
        {
            public:
                static bool run(const ClientStore::Client &_client, InputRouter &_serial, InputNetworkOutput &_error);
        };
    }

    namespace out
    {
        class Response
        {
            public:
                static bool run(Context<data::OutNetworkInput> &_input);
        };
    }
}