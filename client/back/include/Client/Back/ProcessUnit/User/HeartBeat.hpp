#pragma once

#include "Client/Back/ProcessUnit/TransitName.hpp"

#include "Shared/ProcessUnit/AInputProcess.hpp"
#include "Shared/ProcessUnit/IProcessUnitStopable.hpp"
#include "Shared/Log/ILogger.hpp"

namespace pu
{
    class HeartBeatHandler : public AInputProcess<TransitMessage>, IProcessUnitStopable
    {
        public:
            HeartBeatHandler(QueueMessage &_tcp_output);
            virtual ~HeartBeatHandler() = default;

        protected:
            void onInput(InputType _input) final;

            void onStop() final;

        private:
            bool handleTestRequest(InputType &_input);
            bool handleHeartBeat(InputType &_input);

            void heartbeatLoop(std::stop_token _st);

            std::jthread m_thread;

            QueueMessage &m_tcp_output;
    };
}