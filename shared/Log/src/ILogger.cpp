#include "Shared/Log/ILogger.hpp"

namespace logger
{
    void ILogger::log(Level _lvl, const std::string &_content)
    {
        newEventLog({ _lvl, std::chrono::system_clock::now(), _content });
    }
}