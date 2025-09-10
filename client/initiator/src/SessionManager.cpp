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
    auto it = std::find_if(m_session.begin(), m_session.end(), [_front] (const std::shared_ptr<Session> &_session) {
        return _session->getFrontSocket() == _front;
    });
    if (it == m_session.end())
        return nullptr;
    return *it;
}

std::shared_ptr<Session> SessionManager::findSession(const std::shared_ptr<net::UnixStream> &_back)
{
    auto it = std::find_if(m_session.begin(), m_session.end(), [_back] (const std::shared_ptr<Session> &_session) {
        return _session->getBackSocket() == _back;
    });
    if (it == m_session.end())
        return nullptr;
    return *it;
}

std::shared_ptr<Session> SessionManager::findSession(const std::string &_apikey)
{
    auto it = std::find_if(m_session.begin(), m_session.end(), [_apikey] (const std::shared_ptr<Session> &_session) {
        return _session->getApiKey() == _apikey;
    });
    if (it == m_session.end())
        return nullptr;
    return *it;
}