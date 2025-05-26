#include <format>
#include <iostream>
#include <filesystem>

#include "Common/Log/Imp/File.hpp"
#include "Common/Log/Manager.hpp"

namespace log::imp
{
    File::File(const std::string &_name)
        : Base(_name)
    {
        std::filesystem::path path(std::format("./logs/{}.log", _name));
        std::filesystem::path dir = path.parent_path();

        // Create directories if they don't exist
        if (!std::filesystem::exists(dir)) {
            if (!std::filesystem::create_directories(dir)) {
                std::cerr << "Failed to create directory: " << dir << std::endl;
            }
        }
        m_stream.open(path);
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