#pragma once

#include "Server/Core/ProcessUnit/interface/IProcessUnit.hpp"
#include "Server/Core/ProcessUnit/Naming.hpp"

#include "Common/Thread/Pool.hpp"

#if !defined(PU_LOGOUT_TP_SIZE) || PU_LOGOUT_TP_SIZE <= 0
    #define PU_LOGOUT_TP_SIZE 1
#endif

namespace data
{
    using LogoutInput = RouterInput;
}

namespace pu::user
{
    class LogoutHandler : public IProcessUnit<Context<data::LogoutInput>>
    {
        public:
            LogoutHandler(InOutNetwork &_tcp_output);
            virtual ~LogoutHandler() = default;

            QueueInputType &getInput();

        protected:
            void runtime(std::stop_token _st);

        private:
            QueueInputType m_input;
            InOutNetwork &m_tcp_output;

            ThreadPool<PU_LOGOUT_TP_SIZE> m_tp;
    };
}