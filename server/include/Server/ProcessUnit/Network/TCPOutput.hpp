#pragma once

#include "Server/meta.hpp"
#include "Server/ProcessUnit/data/Global.hpp"

#include "Shared/ProcessUnit/AInputProcess.hpp"
#include "Shared/Thread/Pool.hpp"
#include "Shared/Log/ILogger.hpp"

#ifndef PU_NETOUT_TP_SIZE
    #define PU_NETOUT_TP_SIZE 10
#endif

namespace pu
{
    class TcpOutputNetwork : public AInputProcess<Context<data::OutNetworkInput>>
    {
        public:
            using Client = net::Acceptor<net::INetTcp>::Client;

            TcpOutputNetwork();
            virtual ~TcpOutputNetwork() = default;

        protected:
            void onInput(InputType _input) final;

        private:
            ThreadPool<PU_NETOUT_TP_SIZE> m_tp;
    };
}