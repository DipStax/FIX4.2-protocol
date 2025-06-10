#include "Client/Back/FrontManager.hpp"

FrontManager &FrontManager::Instance()
{
    static FrontManager instance{};

    return instance;
}

void FrontManager::wait_frontend()
{
    net::Acceptor<net::UnixStream> acceptor{};

    acceptor.listen("/tmp/fix.backend.socket");

    m_socket = acceptor.accept();
}

void FrontManager::notify(const net::Buffer &_buffer)
{
    m_socket->send(_buffer.data(), _buffer.size());
}