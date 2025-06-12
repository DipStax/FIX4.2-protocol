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

    m_socket = std::make_shared<net::UnixStream>();
    while (!m_socket->connect("/tmp/fix.backend.socket")) {
        Logger->log<logger::Level::Error>("Unable to connect to the backend, retrying in 5s");
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        Logger->log<logger::Level::Info>("Retrying to connect");
    }

    selector.timeout(1000);

    while (true) {
        std::vector<net::Selector<net::UnixStream>::Client> clients = selector.pull();

        if (!clients.empty()) {
            int error;

            std::vector<std::byte> bytes = clients[0]->receive(sizeof(ipc::Header), error);
            if (error != sizeof(ipc::Header)) {
                Logger->log<logger::Level::Warning>("Unable to read ipc::Header from socket");
                continue; // continue
            }
            net::Buffer buffer;
            ipc::Header header;

            buffer.append(bytes.data(), bytes.size());
            buffer >> header;
            bytes = clients[0]->receive(header.BodySize, error);
            if (error != sizeof(ipc::Header)) {
                Logger->log<logger::Level::Warning>("Unable to read body size from socket expected: ", header.BodySize, ", got: ", error);
                continue; // continue
            }
            buffer.append(bytes.data(), bytes.size());
            buffer.reset();

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