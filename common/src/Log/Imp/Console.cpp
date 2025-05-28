#include <format>
#include <iostream>

#include "Common/Log/Imp/Console.hpp"

namespace log::imp
{
    Console::Console(const std::string &_name)
        : Base(_name)
    {
    }

    void Console::newEventLog(Event _event)
    {
        std::stringstream stream;

        stream << "[" << LevelToString(_event.level) << "] (" << Name << ") " << std::format("{:%Y-%m-%d %H:%M:%S}", _event.time) << " - " << _event.content;
        std::cout << stream.str() << std::endl;
    }
}
