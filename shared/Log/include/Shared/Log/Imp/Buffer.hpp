#pragma once

#include <thread>
#include <vector>

#include "Shared/Log/Imp/Base.hpp"
#include "Shared/Thread/Queue.hpp"
#include "Shared/Thread/Pool.hpp"

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
            std::vector<std::unique_ptr<ILogger>> m_loggers;

            inline static ThreadPool<LOG_BUFFER_TP> m_tp{};
    };
}