#pragma once

#include <thread>

#include "Client/Back/ProcessUnit/data/Global.hpp"

#include "Shared/ProcessUnit/AInputProcess.hpp"
#include "Shared/ProcessUnit/IProcessUnitStopable.hpp"
#include "Shared/Log/ILogger.hpp"

namespace pu
{
    class HeartBeatHandler : public AInputProcess<Context<data::UnparsedMessage>>, IProcessUnitStopable
    {
        public:
            HeartBeatHandler(StringOutputQueue &_tcp_output);
            virtual ~HeartBeatHandler() = default;

        protected:
            void onInput(InputType _input) final;

            void setup() final;
            void onStop() final;

        private:
            void handleTestRequest(const InputType &_input);
            void handleHeartBeat(const InputType &_input);

            void heartbeatLoop(std::stop_token _st);

            std::jthread m_thread;

            StringOutputQueue &m_tcp_output;
    };
}