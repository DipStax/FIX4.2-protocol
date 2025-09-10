#pragma once


#include "Server/ProcessUnit/data/Global.hpp"
#include "Server/ClientStore.hpp"
#include "Server/meta.hpp"

#include "Shared/ProcessUnit/AProcessUnitBase.hpp"
#include "Shared/Network/Selector.hpp"

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