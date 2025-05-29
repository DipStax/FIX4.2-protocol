#include <sstream>
#include <format>
#include <iomanip>
#include <ctime>

#include "Client/User.hpp"

#include "Common/Log/Manager.hpp"

User &User::Instance()
{
    static User instance;

    return instance;
}

const std::string &User::getName() const
{
    return m_name;
}

size_t User::getSeqNumber() const
{
    return m_seqnum;
}

std::chrono::system_clock::time_point User::getSinceHeartBeat() const
{
    return m_since_heartbeat;
}

std::chrono::system_clock::time_point User::setSinceHeartBeat(const std::string &_time, const std::string &_format)
{
    std::istringstream stream(_time);
    std::tm tm{};

    stream >> std::get_time(&tm, _format.c_str());
    if (!stream.fail())
        Logger->log<log::Level::Error>("Couldn't convert time for HeartBeat Since");
    m_since_heartbeat = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    Logger->log<log::Level::Debug>("Using HeartBeat Since at: ", std::format("%Y%m%d-%H:%M:%S", m_since_heartbeat));
    return m_since_heartbeat;
}

void User::setSinceHeartBeat(std::chrono::system_clock::time_point _time)
{
    m_since_heartbeat = _time;
}

User::User()
    : Logger(log::Manager::newLogger("UserStore"))
{
}
