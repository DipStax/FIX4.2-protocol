#pragma once

#include "Common/Container/AInputProcess.hpp"
#include "Server/Core/ProcessUnit/data/Global.hpp"

#include "Common/Thread/Pool.hpp"
#include "Common/Log/ILogger.hpp"

#if !defined(PU_LOGON_TP_SIZE) || PU_LOGON_TP_SIZE <= 0
    #define PU_LOGON_TP_SIZE 1
#endif

#if !defined(PU_LOGON_HB_MAX_TO) || PU_LOGON_HB_MAX_TOw <= 0
    #define PU_LOGON_HB_MAX_TO 5.f
#endif

namespace data
{
    using LogonInput = RouterInput;
}

namespace pu::user
{
    class LogonHandler : public AInputProcess<Context<data::LogonInput>>
    {
        public:
            LogonHandler(InputNetworkOutput &_tcp_output);
            virtual ~LogonHandler() = default;

        protected:
            void onInput(InputType _input) final;

        private:
            bool process(InputType &_input);

            QueueInputType m_input;
            InputNetworkOutput &m_tcp_output;

            ThreadPool<PU_LOGON_TP_SIZE> m_tp;
    };
}