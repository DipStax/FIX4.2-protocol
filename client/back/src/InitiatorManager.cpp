#include "Client/Back/InitiatorManager.hpp"
#include "Client/Back/FrontManager.hpp"

#include "Client/Shared/IPC/Message/Identify.hpp"
#include "Client/Shared/IPC/Header.hpp"
#include "Client/Shared/IPC/Helper.hpp"

InitiatorManager &InitiatorManager::Instance()
{
    static InitiatorManager instance{};

    return instance;
}

void InitiatorManager::connect()
{
    const char *apikey = std::getenv("FIX42-apikey");
    const char *address = std::getenv("FIX42-initiator-address");

    m_socket = std::make_shared<net::UnixStream>();

    while (m_socket->connect(address)) {
        Logger->log<logger::Level::Error>("Unable to connect to the initiator, retrying in 5s");
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    }
    Logger->log<logger::Level::Info>("Successfully connected to initiator");

    uint32_t port = FrontManager::Instance().initAcceptor();
    ipc::msg::AuthBackToInitiator auth{apikey};

    send(ipc::Helper::Auth::BackToInitiator(auth));
    m_thread = std::jthread(&InitiatorManager::receiveLoop, this);
}

InitiatorManager::InitiatorManager()
    : IPCNetworkManager<net::UnixStream>("Back/InitiatorManager")
{
}

void InitiatorManager::onError(int _errno)
{
    Logger->log<logger::Level::Fatal>("Error when receiving ", strerror(_errno));
}

void InitiatorManager::onDisconnection()
{
    Logger->log<logger::Level::Fatal>("Initiator disconnected");
}

void InitiatorManager::onWrongSize(const std::vector<std::byte> &_byte, int _readsize)
{
    Logger->log<logger::Level::Error>("Wrong header size received from Initiator");
}

void InitiatorManager::onWrongBodySize(const std::vector<std::byte> &_byte, int _readsize)
{
    Logger->log<logger::Level::Error>("Wrong body size received from Initiator");
}

void InitiatorManager::onReceive(net::Buffer &_buffer)
{
    ipc::Header header;

    ipc::msg::AuthInitiatorToBack auth{};

    _buffer >> header;
    switch (header.MsgType) {
        case ipc::MessageType::InitiatorToBackAuth:
            _buffer >> auth;
            FrontManager::Instance().setupToken(auth.token);
            break;
    }
}