#pragma once

#include "Common/Container/IProcessUnit.hpp"
#include "Common/Message/Fix.hpp"
#include "Common/Log/ILogger.hpp"
#include "Common/Network/Socket.hpp"

namespace pu
{
    class TcpOutputNetwork : public IProcessUnit<fix::Message>
    {
        public:
            using Socket = std::shared_ptr<net::tcp::Socket>;

            TcpOutputNetwork(Socket _server);
            virtual ~TcpOutputNetwork() = default;

            QueueInputType &getInput();

        protected:
            void runtime(std::stop_token _st) final;

        private:
            Socket m_server;

            QueueInputType m_input;

            std::unique_ptr<log::ILogger> Logger = nullptr;
    };
}