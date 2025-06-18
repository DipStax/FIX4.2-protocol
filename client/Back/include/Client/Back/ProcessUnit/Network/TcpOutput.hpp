#pragma once

#include "Common/Container/AInputProcess.hpp"
#include "Common/Message/Fix.hpp"
#include "Common/Log/ILogger.hpp"
#include "Common/Network/Socket.hpp"

namespace pu
{
    class TcpOutputNetwork : public AInputProcess<fix::Message>
    {
        public:
            using Socket = std::shared_ptr<net::INetTcp>;

            TcpOutputNetwork(Socket _server);
            virtual ~TcpOutputNetwork() = default;

        protected:
            void onInput(InputType _input) final;

        private:
            Socket m_server;
    };
}