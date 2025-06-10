#pragma once

#include <memory>

#include "Client/Back/ProcessUnit/TransitName.hpp"

#include "Common/Container/IProcessUnit.hpp"
#include "Common/Log/ILogger.hpp"
#include "Common/Network/Selector.hpp"
#include "Common/Network/Socket.hpp"

namespace pu
{
    class TcpInputNetwork : public IProcessUnitBase
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

            std::unique_ptr<log::ILogger> Logger = nullptr;
    };
}