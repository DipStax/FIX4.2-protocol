#pragma once

#include <thread>
#include <vector>

#include "Common/Log/Imp/Base.hpp"
#include "Common/Thread/Queue.hpp"
#include "Common/Thread/Pool.hpp"

#if !defined(LOG_BUFFER_TP) || LOG_BUFFER_TP < 1
    #define LOG_BUFFER_TP
#endif

namespace logger::imp
{
    class Buffer : public Base
    {
        public:
            Buffer(const std::string &_name);
            ~Buffer() = default;

            void newEventLog(Event _event);

        private:
            static void thread(std::stop_token _st);

            std::vector<std::unique_ptr<ILogger>> m_loggers;

            inline static ThreadPool<LOG_BUFFER_TP> m_tp{};
    };
}