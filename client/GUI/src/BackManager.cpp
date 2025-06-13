#include "Client/GUI/BackManager.hpp"

#include "Client/Common/ipc/Header.hpp"

#include "Common/Log/Manager.hpp"
#include "Common/Network/Selector.hpp"

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

void BackManager::startConnection()
{
    net::Selector<net::UnixStream> selector;
    const std::string socket_address = "/tmp/fix-backend.socket";

    m_socket = std::make_shared<net::UnixStream>();
    Logger->log<logger::Level::Debug>("Connecting to: ", socket_address);
    while (!m_socket->connect(socket_address)) {
        Logger->log<logger::Level::Error>("Unable to connect to the backend, retrying in 5s");
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    }
    Logger->log<logger::Level::Info>("Successfully connected to backend");

    selector.timeout(1000);
    selector.client(m_socket);

    while (true) {
        std::vector<net::Selector<net::UnixStream>::Client> clients = selector.pull();

        if (!clients.empty()) {
            int error = 0;

            std::vector<std::byte> bytes = clients[0]->receive(sizeof(ipc::Header), error);
            if (error != sizeof(ipc::Header)) {
                if (error == -1)
                    Logger->log<logger::Level::Error>("Error when receivin data from back: ", strerror(errno));
                else if (error == 0)
                    Logger->log<logger::Level::Error>("Backend disconnected");
                else
                    Logger->log<logger::Level::Warning>("Unable to read ipc::Header from socket, read size: ", error, " != ", sizeof(ipc::Header));
                continue; // continue
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
            emit received(buffer);
        }
    }
}

void BackManager::send(const net::Buffer &_buffer)
{
    m_socket->send(_buffer.data(), _buffer.size());
}

BackManager::BackManager(QObject *_parent)
    : QObject(_parent), Logger(logger::Manager::newLogger("BackManager"))
{
}