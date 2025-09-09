#pragma once

#include <memory>
#include <vector>

#include "Client/Initiator/Session.hpp"

class SessionManager
{
    public:
        static SessionManager &Instance();

        void newSession(const std::shared_ptr<net::INetTcp> &_front);

        std::shared_ptr<Session> findSession(const std::shared_ptr<net::INetTcp> &_front);
        std::shared_ptr<Session> findSession(const std::shared_ptr<net::UnixStream> &_back);
        std::shared_ptr<Session> findSession(const std::string &_apikey);


    private:
        std::vector<std::shared_ptr<Session>> m_session;
};