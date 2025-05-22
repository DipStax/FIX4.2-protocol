#include <format>

#include "Common/Log/Imp/File.hpp"
#include "Common/Log/Manager.hpp"

namespace log::imp
{
    File::File(const std::string &_name)
        : Base(_name)
    {
        m_stream.open(std::format("./logs/{}.log", _name));
    }

    File::~File()
    {
        m_stream.close();
    }

    void File::newEventLog(Event _event)
    {
        std::stringstream stream;

        stream << "[" << LevelToString(_event.level) << "] (" << Name << ") " << std::format("{:%Y-%m-%d %H:%M:%S}", std::chrono::floor<std::chrono::seconds>(_event.time)) << " - " << _event.content;
        m_stream << stream.str() << std::endl;
    }
}