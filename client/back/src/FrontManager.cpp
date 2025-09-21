#include "Client/Back/FrontManager.hpp"

#include "Client/Shared/IPC/Header.hpp"
#include "Client/Shared/IPC/Helper.hpp"

#include "Shared/Log/Manager.hpp"
#include "Shared/Network/Selector.hpp"
#include "Shared/Utils/Utils.hpp"

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

    Logger->log<logger::Level::Info>("Received new data from Frontend, with message type: ", (int)header.MsgType);
    if (header.MsgType == ipc::MessageType::FrontToBackValidToken) {
        if (m_auth) {
            Logger->log<logger::Level::Warning>("Frontend already authenticated");
            // send reject
        } else {
            if (m_token.has_value()) {
                ipc::msg::FrontToBackValidToken token_front{};

                _buffer >> token_front;
                if (token_front.token == m_token.value()) {
                    ipc::msg::BackToFrontValidToken token_back{token_front.token};

                    Logger->log<logger::Level::Info>("Token validated, sending validation: ", token_back);
                    send(ipc::Helper::ValidationToken::BackToFront(token_back));
                    m_auth = true;
                } else {
                    Logger->log<logger::Level::Fatal>("");
                }
            } else {
                Logger->log<logger::Level::Error>("Token not set, will frontend try to validate it's token");
                // send reject
            }
        }
        return;
    } else if (!m_auth) {
        Logger->log<logger::Level::Error>("Frontend not authenticated, but trying to send other message type");
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
    Logger->log<logger::Level::Debug>("Authenticate with token: ", utils::trunc(_token));
    m_token = _token;
    m_socket = m_acceptor.accept();
    Logger->log<logger::Level::Debug>("Accepted frontend connection");
    m_thread = std::jthread(&FrontManager::receiveLoop, this);
}

FrontManager::FrontManager()
    : IPCNetworkManager<net::INetTcp>("Back/FrontManager")
{
}
