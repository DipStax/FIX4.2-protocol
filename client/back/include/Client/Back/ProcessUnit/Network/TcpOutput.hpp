#pragma once

#include "Shared/ProcessUnit/AInputProcess.hpp"
#include "Shared/Message/Fix.hpp"
#include "Shared/Log/ILogger.hpp"
#include "Shared/Network/Socket.hpp"

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