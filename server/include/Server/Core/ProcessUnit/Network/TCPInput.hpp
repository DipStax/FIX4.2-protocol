#pragma once

#include "Common/Network/Selector.hpp"

#include "Server/Core/ProcessUnit/interface/IProcessUnit.hpp"
#include "Server/Core/ProcessUnit/data/Global.hpp"
#include "Server/Core/ClientStore.hpp"
#include "Server/Core/meta.hpp"

#include "Common/Log/ILogger.hpp"

#ifndef NET_RECV_SIZE
    #define NET_RECV_SIZE 4096
#endif

namespace pu
{
    class TCPInputNetwork : public IProcessUnitBase
    {
        public:
            TCPInputNetwork(InputRouter &_output, InputNetworkOutput &_error, uint32_t _port);
            virtual ~TCPInputNetwork() = default;

        protected:
            void runtime(std::stop_token _st);

        private:
            bool process(const ClientStore::Client &_client);

            InputRouter &m_output;
            InputNetworkOutput &m_error;

            net::Acceptor<net::StreamTcp> m_acceptor{};
            net::Selector<net::StreamTcp> m_selector{};

            std::unique_ptr<log::ILogger> Logger = nullptr;
    };
}