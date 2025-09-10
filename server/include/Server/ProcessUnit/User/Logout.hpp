#pragma once

#include "Shared/ProcessUnit/AInputProcess.hpp"
#include "Server/ProcessUnit/data/Global.hpp"

#include "Shared/Thread/Pool.hpp"
#include "Shared/Log/ILogger.hpp"

#if !defined(PU_LOGOUT_TP_SIZE) || PU_LOGOUT_TP_SIZE <= 0
    #define PU_LOGOUT_TP_SIZE 1
#endif

namespace data
{
    using LogoutInput = RouterInput;
}

namespace pu::user
{
    class LogoutHandler : public AInputProcess<Context<data::LogoutInput>>
    {
        public:
            LogoutHandler(InputNetworkOutput &_tcp_output);
            virtual ~LogoutHandler() = default;

        protected:
            void onInput(InputType _input) final;

        private:
            bool process(InputType &_input);

            InputNetworkOutput &m_tcp_output;

            ThreadPool<PU_LOGOUT_TP_SIZE> m_tp;
    };
}