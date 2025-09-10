#pragma once

#include <thread>
#include <optional>

#include "Shared/ProcessUnit/AInputProcess.hpp"
#include "Shared/ProcessUnit/IProcessUnitStopable.hpp"
#include "Server/ProcessUnit/data/Global.hpp"

#include "Shared/Thread/Pool.hpp"
#include "Shared/Log/ILogger.hpp"

#if !defined(PU_HEARTBEAT_TP_SIZE) || PU_HEARTBEAT_TP_SIZE <= 0
    #define PU_HEARTBEAT_TP_SIZE 1
#endif

namespace data
{
    using HeartBeatInput = RouterInput;
}

namespace pu::user
{
    class HeartBeatHandler : public AInputProcess<Context<data::HeartBeatInput>>, public IProcessUnitStopable
    {
        public:
            HeartBeatHandler(InputNetworkOutput &_tcp_output);
            virtual ~HeartBeatHandler() = default;

        protected:
            void onInput(InputType _input) final;

            void onStop() final;

        private:
            bool processHeartBeat(const InputType &_input);

            void handle(std::stop_token _st);

            std::jthread m_thread;

            InputNetworkOutput &m_tcp_output;

            ThreadPool<PU_HEARTBEAT_TP_SIZE> m_tp;
    };
}