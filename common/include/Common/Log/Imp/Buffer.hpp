#pragma once

#include <thread>
#include <vector>

#include "Common/Log/Imp/Base.hpp"
#include "Common/Thread/Queue.hpp"

namespace log::imp
{
    class Buffer : public Base
    {
        public:
            Buffer(const std::string &_name);
            ~Buffer();

            void newEventLog(Event _event);

        private:
            void thread(std::stop_token _st);

            std::jthread m_thread;

            std::vector<std::unique_ptr<ILogger>> m_loggers;

            ts::Queue<Event> m_queue;
    };
}