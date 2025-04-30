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

        // namespace priv
        // {
        //     void LogTiming(ClientStore::Client _client);
        // }

        class Response
        {
            public:
                static bool run(Context<OutNetworkInput> &_input);
        };

        class SubResponse
        {
            public:
                static bool run(Context<OutNetworkInput> &_input);
        };
    }
}