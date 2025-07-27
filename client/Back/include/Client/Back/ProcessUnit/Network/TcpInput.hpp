#pragma once

#include <memory>

#include "Client/Back/ProcessUnit/TransitName.hpp"

#include "Shared/ProcessUnit/AProcessUnitBase.hpp"
#include "Shared/Log/ILogger.hpp"
#include "Shared/Network/Selector.hpp"
#include "Shared/Network/Socket.hpp"

namespace pu
{
    class TcpInputNetwork : public AProcessUnitBase
    {
        public:
            using Socket = std::shared_ptr<net::INetTcp>;

            TcpInputNetwork(Socket _server, QueueTransit &_output);
            virtual ~TcpInputNetwork() = default;

        protected:
            void runtime(std::stop_token _st)  final;

        private:
            Socket m_server;

            QueueTransit &m_output;

            net::Selector<net::INetTcp> m_selector;
    };
}