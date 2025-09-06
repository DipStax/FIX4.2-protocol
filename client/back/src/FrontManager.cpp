#include "Client/Back/FrontManager.hpp"

#include "Client/Shared/IPC/Header.hpp"
#include "Client/Shared/IPC/Helper.hpp"

#include "Shared/Log/Manager.hpp"
#include "Shared/Network/Selector.hpp"

FrontManager &FrontManager::Instance()
{
    static FrontManager instance{};

    return instance;
}

void FrontManager::wait_frontend()
{
    while (!m_auth) {}
}

ts::Queue<net::Buffer> &FrontManager::getMessageQueue()
{
    return m_output;
}

void FrontManager::onError(int _errno)
{
    Logger->log<logger::Level::Fatal>("Error when receiving ", strerror(_errno));
}

void FrontManager::onDisconnection()
{
    Logger->log<logger::Level::Fatal>("Frontend disconnected");
}

void FrontManager::onWrongSize(const std::vector<std::byte> &_byte, int _readsize)
{
    Logger->log<logger::Level::Error>("Wrong header size received from Frontend");
}

void FrontManager::onWrongBodySize(const std::vector<std::byte> &_byte, int _readsize)
{
    Logger->log<logger::Level::Error>("Wrong body size received from Frontend");
}

void FrontManager::onReceive(net::Buffer &_buffer)
{
    ipc::Header header;

    _buffer >> header;

    if (header.MsgType == ipc::MessageType::FrontToBackAuth) {
        if (m_auth) {
            // send reject
        } else {
            if (m_token.has_value()) {
                ipc::msg::AuthFrontToBack authfront{};

                _buffer >> authfront;
                if (authfront.token == m_token.value()) {
                    ipc::msg::AuthBackToFront authback{authfront.token};

                    send(ipc::Helper::Auth::BackToFront(authback));
                    m_auth = true;
                } else {
                    // send reject
                }
            } else {
                // send reject
            }
        }
        return;
    } else if (!m_auth) {
        // send reject
        return;
    }
    _buffer.reset();
    m_output.push(std::move(_buffer));
}

uint32_t FrontManager::initAcceptor()
{
    if (!m_acceptor.listen(0)) {
        Logger->log<logger::Level::Error>("Acceptor failed to listen, error: ", strerror(errno));
        return 0;
    }
    Logger->log<logger::Level::Info>("Waiting connection of client on socket: 127.0.0.1:", m_acceptor.getPort());
    return m_acceptor.getPort();
}

void FrontManager::setupToken(const std::string &_token)
{
    m_token = _token;
    m_socket = m_acceptor.accept();
    m_thread = std::jthread(&FrontManager::receiveLoop, this);
}

FrontManager::FrontManager()
    : IPCNetworkManager<net::INetTcp>("Client/FrontManager")
{
}
