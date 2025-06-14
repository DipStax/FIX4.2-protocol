#include <sstream>
#include <format>
#include <iomanip>
#include <chrono>

#include "Client/Back/User.hpp"

#include "Common/Log/Manager.hpp"

User &User::Instance()
{
    static User instance;

    return instance;
}

void User::setSeqNumber(size_t _seq_num)
{
    m_seqnum = _seq_num;
}

void User::nextSeqNumber()
{
    m_seqnum++;
}

size_t User::getSeqNumber() const
{
    return m_seqnum;
}

void User::login(const std::string &_userid)
{
    m_userid = _userid;
    m_login = true;
}

bool User::isLogin() const
{
    return m_login;
}

const std::string &User::getUserId() const
{
    return m_userid;
}

void User::setUserId(const std::string &_userid)
{
    m_userid = _userid;
}


void User::logoutRequested(bool _logout)
{
    m_logout_req = _logout;
}

bool User::logoutRequested() const
{
    return m_logout_req;
}

void User::shouldDisconnect(bool _dc)
{
    m_dc_req = _dc;
}

bool User::shouldDisconnect() const
{
    return m_dc_req;
}

void User::disconnect()
{
    m_userid = "";
    m_login = false;
    m_dc_req = false;
    m_logout_req = false;
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
    : Logger(logger::Manager::newLogger("UserStore"))
{
}
