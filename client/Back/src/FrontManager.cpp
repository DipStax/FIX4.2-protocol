#include "Client/Back/FrontManager.hpp"

#include "Common/Log/Manager.hpp"

FrontManager &FrontManager::Instance()
{
    static FrontManager instance{};

    return instance;
}

void FrontManager::wait_frontend()
{
    net::Acceptor<net::UnixStream> acceptor{};

    acceptor.listen("/tmp/fix.backend.socket");
    Logger->log<logger::Level::Info>("Waiting connection of client on socket: /tmp/fix.backend.socket");
    while (!m_socket)
        m_socket = acceptor.accept();
    Logger->log<logger::Level::Info>("Client front connected to server", m_socket == nullptr);
}

void FrontManager::notify(const net::Buffer &_buffer)
{
    m_socket->send(_buffer.data(), _buffer.size());
}

FrontManager::FrontManager()
    : Logger(logger::Manager::newLogger("FrontManager"))
{
}