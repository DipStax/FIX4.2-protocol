#include <format>
#include <iostream>
#include <filesystem>

#include "Shared/Log/Imp/File.hpp"
#include "Shared/Log/Manager.hpp"

namespace logger::imp
{
    File::File(const std::string &_name)
        : Base(_name)
    {
    }

    void File::Init(const std::string _file)
    {
        std::filesystem::path path(std::format("{}.0.log", _file));
        std::filesystem::path dir = path.parent_path();

        if (!std::filesystem::exists(dir))
            if (!std::filesystem::create_directories(dir))
                std::cerr << "Failed to create directory: " << dir << std::endl;
        if (std::filesystem::exists(path))
            moveFile(path.parent_path() / path.stem().stem());
        m_stream.open(path);
    }

    void File::Deinit()
    {
        m_stream.close();
    }

    void File::moveFile(const std::filesystem::path &_path, size_t _it)
    {
        std::filesystem::path fullpath(std::format("{}.{}.log", _path.string(), _it));

        if (std::filesystem::exists(fullpath))
            moveFile(_path, _it + 1);
        std::rename(fullpath.string().c_str(), std::format("{}.{}.log", _path.string(), _it + 1).c_str());
    }

    File::~File()
    {
    }

    void File::newEventLog(Event _event)
    {
        std::stringstream stream;

        stream << "[" << LevelToString(_event.level) << "] (" << Name << ") " << std::format("{:%Y-%m-%d %H:%M:%S}", std::chrono::floor<std::chrono::seconds>(_event.time)) << " - " << _event.content;
        {
            std::lock_guard lock(m_mutex);

            m_stream << stream.str() << std::endl;
        }
    }
}