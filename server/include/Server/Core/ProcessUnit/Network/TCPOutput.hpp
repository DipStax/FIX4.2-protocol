#pragma once

#include "Server/Core/meta.hpp"
#include "Server/Core/ProcessUnit/data/Global.hpp"

#include "Common/Container/AInputProcess.hpp"
#include "Common/Thread/Pool.hpp"
#include "Common/Log/ILogger.hpp"

#ifndef TS_SIZE_ON
    #define TS_SIZE_ON 10
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
            ThreadPool<TS_SIZE_ON> m_tp;
    };
}