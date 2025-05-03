#pragma once

#include "Server/Core/ProcessUnit/interface/IProcessUnit.hpp"
#include "Server/Core/ProcessUnit/Naming.hpp"

#include "Common/Thread/Pool.hpp"

#if !defined(PU_LOGON_TP_SIZE) || PU_LOGON_TP_SIZE <= 0
    #define PU_LOGON_TP_SIZE 1
#endif

namespace data
{
    using LogonInput = RouterInput;
}

namespace pu::user
{
    class LogonHandler : public IProcessUnit<Context<data::LogonInput>>
    {
        public:
            LogonHandler(InOutNetwork &_tcp_output);
            virtual ~LogonHandler() = default;

            QueueInputType &getInput();

        protected:
            void runtime(std::stop_token _st);

        private:
            QueueInputType m_input;
            InOutNetwork &m_tcp_output;

            ThreadPool<PU_LOGON_TP_SIZE> m_tp;
    };
}