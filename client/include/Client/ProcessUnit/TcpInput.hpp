#pragma once

#include <memory>

#include "Client/ProcessUnit/TransitName.hpp"

#include "Common/Container/IProcessUnit.hpp"
#include "Common/Log/ILogger.hpp"
#include "Common/Network/Selector.hpp"
#include "Common/Network/Socket.hpp"

namespace pu
{
    class TcpInputNetwork : public IProcessUnitBase
    {
        public:
            using Socket = std::shared_ptr<net::tcp::Socket>;

            TcpInputNetwork(Socket _server, QueueTransit &_output);
            virtual ~TcpInputNetwork() = default;

        protected:
            void runtime(std::stop_token _st) override;

        private:
            Socket m_server;

            QueueTransit &m_output;

            net::Selector<net::tcp::Socket> m_selector;

            std::unique_ptr<log::ILogger> Logger = nullptr;
    };
}