#include "Client/Back/FrontManager.hpp"

#include "Common/Log/Manager.hpp"

FrontManager &FrontManager::Instance()
{
    static FrontManager instance{};

    return instance;
}

bool FrontManager::wait_frontend()
{
    if (!m_acceptor.listen("/tmp/fix-backend.socket")) {
        Logger->log<logger::Level::Error>("Acceptor failed to listen, error: ", strerror(errno));
        return false;
    }
    Logger->log<logger::Level::Info>("Waiting connection of client on socket: /tmp/fix.backend.socket");
    m_socket = m_acceptor.accept();
    if (m_socket == nullptr) {
        Logger->log<logger::Level::Error>("Acceptor failed to accept, error: ", strerror(errno));
        return false;
    }
    Logger->log<logger::Level::Info>("Client front connected to server");
    return true;
}

void FrontManager::notify(const net::Buffer &_buffer)
{
    m_socket->send(_buffer.data(), _buffer.size());
}

FrontManager::FrontManager()
    : Logger(logger::Manager::newLogger("FrontManager"))
{
}