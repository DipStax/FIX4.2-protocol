#pragma once

#include <thread>
#include <optional>

#include "Common/Container/IProcessUnit.hpp"
#include "Common/Container/IProcessUnitStopable.hpp"
#include "Server/Core/ProcessUnit/data/Global.hpp"

#include "Common/Thread/Pool.hpp"
#include "Common/Log/ILogger.hpp"

#if !defined(PU_HEARTBEAT_TP_SIZE) || PU_HEARTBEAT_TP_SIZE <= 0
    #define PU_HEARTBEAT_TP_SIZE 1
#endif

#if !defined(PU_HEARTBEAT_TO) || PU_HEARTBEAT_TO <= 0
    #define PU_HEARTBEAT_TO 5.f
#endif

namespace data
{
    using HeartBeatInput = RouterInput;
}

namespace pu::user
{
    class HeartBeatHandler : public IProcessUnit<Context<data::HeartBeatInput>>, public IProcessUnitStopable
    {
        public:
            HeartBeatHandler(InputNetworkOutput &_tcp_output);
            virtual ~HeartBeatHandler() = default;

            QueueInputType &getInput();

        protected:
            void runtime(std::stop_token _st);

            void onStop();

        private:
            bool processHeartBeat(const InputType &_input);

            void handle(std::stop_token _st);

            std::jthread m_thread;

            QueueInputType m_input;

            InputNetworkOutput &m_tcp_output;

            ThreadPool<PU_HEARTBEAT_TP_SIZE> m_tp;

            std::unique_ptr<log::ILogger> Logger = nullptr;
    };
}