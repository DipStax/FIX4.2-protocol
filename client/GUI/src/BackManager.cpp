#include <QThread>

#include "Client/GUI/BackManager.hpp"
#include "Client/GUI/Config.hpp"

#include "Client/Shared/IPC/Header.hpp"

#include "Shared/Log/Manager.hpp"
#include "Shared/Network/Selector.hpp"


BackManager *BackManager::Instance()
{
    if (!m_instance) {
        m_instance = new BackManager();

        QThread *thread = new QThread();
        m_instance->moveToThread(thread);

        QObject::connect(thread, &QThread::finished, m_instance, &QObject::deleteLater);
        thread->start();
    }
    return m_instance;
}

void BackManager::setTargetPort(uint32_t _port)
{
    m_socket = std::make_shared<net::INetTcp>();
    Logger->log<logger::Level::Debug>("Connecting to: ", Configuration<config::Global>::Get().Config.Back.Ip, ":", _port);
    while (!m_socket->connect(Configuration<config::Global>::Get().Config.Back.Ip, _port)) {
        Logger->log<logger::Level::Error>("Unable to connect to the Initiator, retrying in 5s");
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    }
    Logger->log<logger::Level::Info>("Successfully connected to backend");
}

void BackManager::startConnection()
{
    if (m_running) {
        Logger->log<logger::Level::Warning>("Backend connection is already running");
        return;
    }

    net::Selector<net::INetTcp> selector;

    selector.timeout(1000);
    selector.client(m_socket);
    Logger->log<logger::Level::Info>("Backend connection is now running");
    m_running = true;
    while (m_running) {
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

void BackManager::send(const net::Buffer &_buffer)
{
    m_socket->send(_buffer.data(), _buffer.size());
}

void BackManager::stop()
{
    m_running = false;
}

BackManager::BackManager(QObject *_parent)
    : QObject(_parent), Logger(logger::Manager::newLogger("BackManager"))
{
}

void BackManager::ipcReceived(net::Buffer &_buffer)
{
    ipc::Header header;

    ipc::msg::Logon logon{};
    ipc::msg::Execution exec;
    ipc::msg::BackToFrontValidToken token;
    uint8_t status;

    _buffer >> header;
    Logger->log<logger::Level::Info>("Received new data from Backend, with message type: ", (int)header.MsgType);
    switch (header.MsgType) {
        case ipc::MessageType::BackToFrontValidToken:
            _buffer >> token;
            emit received_TokenValidation(token);
            break;
        case ipc::MessageType::Status:
            _buffer >> status;
            emit received_Status(static_cast<PUStatus>(status));
            break;
        case ipc::MessageType::Logon:
            _buffer >> logon;
            emit received_Logon(logon);
            break;
        case ipc::MessageType::ExecNew:
            _buffer >> exec;
            emit received_ExecutionNew(exec);
            break;
        case ipc::MessageType::ExecEvent:
            _buffer >> exec;
            emit received_ExecutionEvent(exec);
            break;
        default:
            Logger->log<logger::Level::Error>("Unknown received message type: ", static_cast<int>(header.MsgType));
            break;
    }
}