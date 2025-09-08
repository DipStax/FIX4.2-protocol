#include <QThread>

#include "Client/GUI/InitiatorManager.hpp"

#include "Client/Shared/IPC/Header.hpp"

#include "Shared/Log/Manager.hpp"
#include "Shared/Network/Selector.hpp"

InitiatorManager *InitiatorManager::Instance()
{
    if (!m_instance) {
        m_instance = new InitiatorManager();

        QThread *thread = new QThread();
        m_instance->moveToThread(thread);

        QObject::connect(thread, &QThread::finished, m_instance, &QObject::deleteLater);
        thread->start();
    }
    return m_instance;
}


void InitiatorManager::startConnection()
{
    net::Selector<net::INetTcp> selector;

    m_socket = std::make_shared<net::INetTcp>();
    Logger->log<logger::Level::Debug>("Connecting to: 127.0.0.1:8082"); // todo use config
    while (!m_socket->connect("127.0.0.1", 8082)) { // todo use config
        Logger->log<logger::Level::Error>("Unable to connect to the backend, retrying in 5s");
        std::this_thread::sleep_for(std::chrono::milliseconds(5000)); // todo use config
    }
    Logger->log<logger::Level::Info>("Successfully connected to Initiator");
    emit connectionReady();
    receiveLoop(m_stopsource.get_token());
}

void InitiatorManager::send(const net::Buffer &_buffer)
{
    m_socket->send(_buffer.data(), _buffer.size());
}

void InitiatorManager::stop()
{
    m_stopsource.request_stop();
}

InitiatorManager::InitiatorManager(QObject *_parent)
    : QObject(_parent), IPCNetworkManager<net::INetTcp>("InitiatorManager")
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

    ipc::msg::AuthInitiatorToFront auth{};
    ipc::msg::InitiatorToFrontValidToken token{};

    _buffer >> header;
    switch (header.MsgType) {
        case ipc::MessageType::InitiatorToFrontAuth:
            _buffer >> auth;
            emit received_IdentifyFront(auth);
            break;
        case ipc::MessageType::InitiatorToFrontValidToken:
            _buffer >> token;
            emit received_ValidationToken(token);
            break;
        default:
            Logger->log<logger::Level::Error>("Unknown received message type: ", static_cast<int>(header.MsgType));
            break;
    }
}