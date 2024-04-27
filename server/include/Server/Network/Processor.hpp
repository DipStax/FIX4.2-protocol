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
                static bool run(ClientSocket &_client, InAction &_serial, InOutNetwork &_error);
        };
    }

    namespace out
    {

        namespace priv
        {
            using ClientInfo = std::tuple<std::vector<ClientSocket>::iterator, std::string>;

            ClientInfo getClientInfo(OutNetworkInput &_input, std::vector<ClientSocket> &_clients);
            ClientInfo getClientInfo(const std::string &_userId, std::vector<ClientSocket> &_clients);
            ClientInfo getClientInfo(std::shared_ptr<net::tcp::Socket> _socket, std::vector<ClientSocket> &_clients);

            void LogTiming(std::vector<ClientSocket>::iterator _it);
        }

        class Response
        {
            public:
                static bool run(OutNetworkInput &_input, std::vector<ClientSocket> &_clients);
        };

        class SubResponse
        {
            public:
                static bool run(OutNetworkInput &_input, std::vector<ClientSocket> &_clients);
        };
    }
}