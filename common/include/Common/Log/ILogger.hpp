#pragma once

#include "Common/Log/Event.hpp"

namespace log
{
    class ILogger
    {
        public:
            template<Level Lvl, class ...Ts>
            void log(Ts &&..._args);

            virtual void newEventLog(Event _event) = 0;

        private:
            void log(Level _lvl, const std::string &_content);
    };
}

#include "Common/Log/ILogger.inl"