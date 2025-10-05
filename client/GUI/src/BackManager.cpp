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

    _buffer >> header;
    Logger->log<logger::Level::Info>("Received new data from Backend, with message type: ", static_cast<int>(header.MsgType));
    switch (header.MsgType) {
        case ipc::MessageType::BackToFrontValidToken: emit_TokenValidation(_buffer);
            break;
        case ipc::MessageType::Status: emit_Status(_buffer);
            break;
        case ipc::MessageType::Logon: emit_Logon(_buffer);
            break;
        case ipc::MessageType::ExecNew: emit_ExecutionNew(_buffer);
            break;
        case ipc::MessageType::ExecEvent: emit_ExecutionEvent(_buffer);
            break;
        default:
            Logger->log<logger::Level::Error>("Unknown received message type: ", static_cast<int>(header.MsgType));
            break;
    }
}

void BackManager::emit_TokenValidation(net::Buffer &_buffer)
{
    ipc::msg::BackToFrontValidToken token{};

    _buffer >> token;
    Logger->log<logger::Level::Debug>("Emiting TokenValidation with: ", token);
    emit received_TokenValidation(token);
}

void BackManager::emit_Status(net::Buffer &_buffer)
{
    uint8_t status;

    _buffer >> status;
    Logger->log<logger::Level::Debug>("Emiting Status with: ", static_cast<uint16_t>(status));
    emit received_Status(static_cast<PUStatus>(status));
}

void BackManager::emit_Logon(net::Buffer &_buffer)
{
    ipc::msg::Logon logon{};

    _buffer >> logon;
    Logger->log<logger::Level::Debug>("Emiting Logon with: ", logon);
    emit received_Logon(logon);
}

void BackManager::emit_ExecutionNew(net::Buffer &_buffer)
{
    ipc::msg::ExecutionNew exec;

    _buffer >> exec;
    Logger->log<logger::Level::Debug>("Emiting ExecutionNew with: ", exec);
    emit received_ExecutionNew(exec);
}

void BackManager::emit_ExecutionEvent(net::Buffer &_buffer)
{
    ipc::msg::ExecutionEvent exec;

    _buffer >> exec;
    Logger->log<logger::Level::Debug>("Emiting ExecutionEvent with: ", exec);
    emit received_ExecutionEvent(exec);
}