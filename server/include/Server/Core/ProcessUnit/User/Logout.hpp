#pragma once

#include "Common/Container/IProcessUnit.hpp"
#include "Server/Core/ProcessUnit/data/Global.hpp"

#include "Common/Thread/Pool.hpp"
#include "Common/Log/ILogger.hpp"

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
            LogoutHandler(InputNetworkOutput &_tcp_output);
            virtual ~LogoutHandler() = default;

            QueueInputType &getInput();

        protected:
            void runtime(std::stop_token _st);

        private:
            bool process(InputType &&_input);

            QueueInputType m_input;
            InputNetworkOutput &m_tcp_output;

            ThreadPool<PU_LOGOUT_TP_SIZE> m_tp;

            std::unique_ptr<log::ILogger> Logger = nullptr;
    };
}