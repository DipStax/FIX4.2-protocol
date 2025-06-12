#pragma once

#include "Client/Back/ProcessUnit/TransitName.hpp"

#include "Common/Container/IProcessUnit.hpp"
#include "Common/Container/IProcessUnitStopable.hpp"
#include "Common/Log/ILogger.hpp"

#if !defined(PU_HEARTBEAT_TO) || PU_HEARTBEAT_TO < 1
    #define PU_HEARTBEAT_TO 1.f
#endif

namespace pu
{
    class HeartBeatHandler : public IProcessUnit<TransitMessage>, IProcessUnitStopable
    {
        public:
            HeartBeatHandler(QueueMessage &_tcp_output);
            virtual ~HeartBeatHandler() = default;

            QueueInputType &getInput() final;

        protected:
            void runtime(std::stop_token _st) final;

            void onStop() final;

        private:
            bool handleTestRequest(InputType &_input);
            bool handleHeartBeat(InputType &_input);

            void heartbeatLoop(std::stop_token _st);

            std::jthread m_thread;

            QueueInputType m_input;

            QueueMessage &m_tcp_output;

            std::unique_ptr<logger::ILogger> Logger = nullptr;
    };
}