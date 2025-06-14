#pragma once

#include <chrono>
#include <string>

#include "Common/Log/Level.hpp"

namespace logger
{
    struct Event
    {
        Level level;
        std::chrono::system_clock::time_point time;
        std::string content;
    };
}