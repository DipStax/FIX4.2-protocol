#include <iostream>

#include "Common/Core/Logger.hpp"

template<class ...Ts>
void Logger::Log(Ts &&..._args)
{
    std::string name = "[";
    std::thread::id id = std::this_thread::get_id();

    if (m_thread_name.contains(id))
        name += m_thread_name[id];
    else
        name += "No thread name";
    name += "] ";
    LogImpl(m_file, name, std::forward<Ts>(_args)...);
    LogImpl(std::cout, name, std::forward<Ts>(_args)...);
}

template <class... Ts>
void Logger::LogImpl(std::ostream &stream, Ts &&..._args) {
    (stream << ... << std::forward<Ts>(_args)) << '\n';
    stream.flush();
}