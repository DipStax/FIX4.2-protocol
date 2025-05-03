#pragma once

#include "Server/Core/ProcessUnit/interface/IProcessUnit.hpp"
#include "Server/Core/ProcessUnit/Naming.hpp"

#include "Common/Thread/Pool.hpp"

#if !defined(PU_HEARTBEAT_TP_SIZE) || PU_HEARTBEAT_TP_SIZE <= 0
    #define PU_HEARTBEAT_TP_SIZE 1
#endif

namespace data
{
    using HeartBeatInput = RouterInput;
}

namespace pu::user
{
    class HeartBeatHandler : public IProcessUnit<Context<data::HeartBeatInput>>
    {
        public:
            HeartBeatHandler(InOutNetwork &_tcp_output);
            virtual ~HeartBeatHandler() = default;

            QueueInputType &getInput();

        protected:
            void runtime(std::stop_token _st);

            QueueInputType m_input;

            InOutNetwork &m_tcp_output;

            ThreadPool<PU_HEARTBEAT_TP_SIZE> m_tp;
    };
}