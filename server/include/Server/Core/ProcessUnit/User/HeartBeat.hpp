#pragma once

#include <thread>

#include "Server/Core/ProcessUnit/interface/IProcessUnit.hpp"
#include "Server/Core/ProcessUnit/interface/IProcessUnitStopable.hpp"
#include "Server/Core/ProcessUnit/Naming.hpp"

#include "Common/Thread/Pool.hpp"

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
            HeartBeatHandler(InOutNetwork &_tcp_output);
            virtual ~HeartBeatHandler() = default;

            QueueInputType &getInput();

        protected:
            std::string getThreadName() const;

            void runtime(std::stop_token _st);

            void onStop();

        private:
            bool process(InputType &&_input);

            void handle(std::stop_token _st);

            std::jthread m_thread;

            using HeartBeatPair = std::pair<ClientStore::Client, std::chrono::system_clock::time_point>;
            QueueInputType m_input;

            InOutNetwork &m_tcp_output;

            std::shared_mutex m_mutex;
            std::vector<HeartBeatPair> m_heartbeat;

            ThreadPool<PU_HEARTBEAT_TP_SIZE> m_tp;
    };
}