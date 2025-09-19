#pragma once

#include "Client/Back/ProcessUnit/data/Global.hpp"

#include "Shared/ProcessUnit/AInputProcess.hpp"
#include "Shared/Log/ILogger.hpp"
#include "Shared/Network/Socket.hpp"

namespace pu
{
    class TcpOutputNetwork : public AInputProcess<Context<data::StringOutput>>
    {
        public:
            using Socket = std::shared_ptr<net::INetTcp>;

            TcpOutputNetwork(Socket _server);
            virtual ~TcpOutputNetwork() = default;

        protected:
            void onInput(InputType _input) final;

            static void AddCheckSum(std::string &_msg);

        private:
            Socket m_server;
    };
}