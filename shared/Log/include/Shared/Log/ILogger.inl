#include <sstream>

#include "Shared/Log/ILogger.hpp"

namespace logger
{
    template<Level Lvl, class ...Ts>
    void ILogger::log(Ts &&..._content)
    {
        std::stringstream stream;

        (stream << ... << _content);
        log(Lvl, stream.str());
    }
}