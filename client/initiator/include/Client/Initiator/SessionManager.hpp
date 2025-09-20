#pragma once

#include <functional>
#include <memory>
#include <vector>
#include <shared_mutex>

#include "Client/Initiator/Session.hpp"

#include "Shared/Thread/Pool.hpp"

class SessionManager
{
    public:
        using OnSessionCallback = std::function<void(const std::shared_ptr<Session> &)>;

        static SessionManager &Instance();

        void newSession(const std::shared_ptr<net::INetTcp> &_front);

        std::shared_ptr<Session> findSession(const std::shared_ptr<net::INetTcp> &_front);
        std::shared_ptr<Session> findSession(const std::shared_ptr<net::UnixStream> &_back);
        std::shared_ptr<Session> findSession(const std::string &_apikey);

        static void OnRemoveSession(OnSessionCallback _callback);
        void removeSession(const std::shared_ptr<Session> &_session);

    private:
        std::shared_mutex m_session_mutex{};
        std::vector<std::shared_ptr<Session>> m_session;

        static inline std::shared_mutex m_onremove_mutex{};
        static inline std::vector<OnSessionCallback> m_onremove{};
        static inline ThreadPool<1> m_tp_onremove{};
};