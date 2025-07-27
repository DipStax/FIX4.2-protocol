#pragma once


#include "Server/Core/ProcessUnit/data/Global.hpp"
#include "Server/Core/ClientStore.hpp"
#include "Server/Core/meta.hpp"

#include "Shared/ProcessUnit/AProcessUnitBase.hpp"
#include "Shared/Network/Selector.hpp"

#ifndef NET_RECV_SIZE
    #define NET_RECV_SIZE 4096
#endif

namespace pu
{
    class TCPInputNetwork : public AProcessUnitBase
    {
        public:
            TCPInputNetwork(InputRouter &_output, InputNetworkOutput &_error, uint32_t _port);
            virtual ~TCPInputNetwork() = default;

        protected:
            void runtime(std::stop_token _st) final;

        private:
            bool process(const ClientStore::Client &_client);

            InputRouter &m_output;
            InputNetworkOutput &m_error;

            net::Acceptor<net::INetTcp> m_acceptor{};
            net::Selector<net::INetTcp> m_selector{};
    };
}