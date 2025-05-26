#include <sstream>

#include "Common/Log/ILogger.hpp"

namespace log
{
    template<Level Lvl, class ...Ts>
    void ILogger::log(Ts &&..._content)
    {
        std::stringstream stream;

        (stream << ... << _content);
        log(Lvl, stream.str());
    }
}