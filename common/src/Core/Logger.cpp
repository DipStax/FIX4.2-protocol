#include <iostream>

#include "Common/Core/Logger.hpp"

void Logger::SetThreadName(std::thread::id _id, const std::string &_name)
{
    Log("New thread name: [", _id, "] given: ", _name);
    m_thread_name[_id] = _name;
}

void Logger::Init(const std::string &_path)
{
    m_file = std::ofstream(_path, std::ofstream::out | std::ofstream::trunc);
}

