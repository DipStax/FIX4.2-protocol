#include "Client/Initiator/SessionManager.hpp"

SessionManager &SessionManager::Instance()
{
    static SessionManager instance{};

    return instance;
}

void SessionManager::newSession(const std::shared_ptr<net::INetTcp> &_front)
{
    std::unique_lock lock(m_session_mutex);

    m_session.push_back(std::make_shared<Session>(_front));
}

std::shared_ptr<Session> SessionManager::findSession(const std::shared_ptr<net::INetTcp> &_front)
{
    std::shared_lock lock(m_session_mutex);

    auto it = std::find_if(m_session.begin(), m_session.end(), [_front] (const std::shared_ptr<Session> &_session) {
        return _session->getFrontSocket() == _front;
    });
    if (it == m_session.end())
        return nullptr;
    return *it;
}

std::shared_ptr<Session> SessionManager::findSession(const std::shared_ptr<net::UnixStream> &_back)
{
    std::shared_lock lock(m_session_mutex);

    auto it = std::find_if(m_session.begin(), m_session.end(), [_back] (const std::shared_ptr<Session> &_session) {
        return _session->getBackSocket() == _back;
    });
    if (it == m_session.end())
        return nullptr;
    return *it;
}

std::shared_ptr<Session> SessionManager::findSession(const std::string &_apikey)
{
    std::shared_lock lock(m_session_mutex);

    auto it = std::find_if(m_session.begin(), m_session.end(), [_apikey] (const std::shared_ptr<Session> &_session) {
        return _session->getApiKey() == _apikey;
    });
    if (it == m_session.end())
        return nullptr;
    return *it;
}

void SessionManager::OnRemoveSession(OnSessionCallback _callback)
{
    std::unique_lock lock(m_onremove_mutex);

    m_onremove.push_back(_callback);
}

void SessionManager::removeSession(const std::shared_ptr<Session> &_session)
{
    _session->close();
    m_tp_onremove.enqueue([this, _session] () {
        {
            std::unique_lock lock_client(m_session_mutex);

            auto it = std::find_if(m_session.begin(), m_session.end(), [_session] (const std::shared_ptr<Session> _original) {
                return _session == _original;
            });
            m_session.erase(it);
        }
        {
            std::shared_lock lock_remove(m_onremove_mutex);

            for (OnSessionCallback _cb : m_onremove)
                _cb(_session);
        }
    });
}
