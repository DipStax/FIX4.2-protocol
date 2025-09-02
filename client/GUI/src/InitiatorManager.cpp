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

    selector.timeout(1000);
    selector.client(m_socket);

    while (true) {
        std::vector<net::Selector<net::INetTcp>::Client> clients = selector.pull();

        if (!clients.empty()) {
            int error = 0;

            std::vector<std::byte> bytes = clients[0]->receive(sizeof(ipc::Header), error);
            if (error != sizeof(ipc::Header)) {
                if (error == -1)
                    Logger->log<logger::Level::Error>("Error when receivin data from back: ", strerror(errno));
                else if (error == 0)
                    Logger->log<logger::Level::Fatal>("Backend disconnected");
                else
                    Logger->log<logger::Level::Warning>("Unable to read ipc::Header from socket, read size: ", error, " != ", sizeof(ipc::Header));
                continue;
            }
            net::Buffer buffer;
            ipc::Header header;

            buffer.append(bytes.data(), bytes.size());
            buffer >> header;
            bytes = clients[0]->receive(header.BodySize, error);
            if (error != static_cast<int>(header.BodySize)) {
                Logger->log<logger::Level::Warning>("Unable to read body size from socket expected: ", header.BodySize, ", got: ", error);
                continue; // continue
            }
            buffer.append(bytes.data(), bytes.size());
            buffer.reset();

            Logger->log<logger::Level::Debug>("Received new data from backend");
            ipcReceived(buffer);
        }
    }
}

void InitiatorManager::send(const net::Buffer &_buffer)
{
    m_socket->send(_buffer.data(), _buffer.size());
}

InitiatorManager::InitiatorManager(QObject *_parent)
    : QObject(_parent), Logger(logger::Manager::newLogger("InitiatorManager"))
{
}

void InitiatorManager::ipcReceived(net::Buffer &_buffer)
{
    ipc::Header header;

    ipc::msg::IdentifyFront identify{};

    _buffer >> header;
    switch (header.MsgType) {
        case ipc::MessageType::Identify:
            _buffer >> identify;
            emit received_IdentifyFront(identify);
            break;
        default:
            Logger->log<logger::Level::Error>("Unknown received message type: ", static_cast<int>(header.MsgType));
            break;
    }
}