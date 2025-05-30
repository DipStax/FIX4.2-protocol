#pragma once

#include "Server/Core/meta.hpp"
#include "Server/Core/ProcessUnit/data/Global.hpp"
#include "Server/Core/ProcessUnit/interface/IProcessUnit.hpp"

#include "Common/Thread/Pool.hpp"
#include "Common/Log/ILogger.hpp"

#ifndef TS_SIZE_ON
    #define TS_SIZE_ON 10
#endif

namespace pu
{
    class TcpOutputNetwork : public IProcessUnit<Context<data::OutNetworkInput>>
    {
        public:
            using Client = net::Acceptor<net::tcp::Socket>::Client;

            TcpOutputNetwork();
            virtual ~TcpOutputNetwork() = default;

            [[nodiscard]] QueueInputType &getInput();

        protected:
            void runtime(std::stop_token _st);

        private:
            QueueInputType m_input;

            ThreadPool<TS_SIZE_ON> m_tp;

            std::unique_ptr<log::ILogger> Logger = nullptr;
    };
}