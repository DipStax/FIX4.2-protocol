#include <sstream>
#include <format>
#include <iomanip>
#include <chrono>



#include "Client/User.hpp"

#include "Common/Log/Manager.hpp"

User &User::Instance()
{
    static User instance;

    return instance;
}

const std::string &User::getUserId() const
{
    return m_userid;
}

void User::setSeqNumber(size_t _seq_num)
{
    m_seqnum = _seq_num;
}

size_t User::nextSeqNumber()
{
    size_t tmp = m_seqnum;

    m_seqnum++;
    return tmp;
}

size_t User::getSeqNumber() const
{
    return m_seqnum;
}

bool User::isLogin() const
{
    return m_login;
}

std::chrono::system_clock::time_point User::getSinceHeartBeat() const
{
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
