#include "Client/Initiator/SessionManager.hpp"

SessionManager &SessionManager::Instance()
{
    static SessionManager instance{};

    return instance;
}

void SessionManager::newSession(const std::shared_ptr<net::INetTcp> &_front)
{
    m_session.push_back(std::make_shared<Session>(_front));
}

std::shared_ptr<Session> SessionManager::findSession(const std::shared_ptr<net::INetTcp> &_front)
{
    
}