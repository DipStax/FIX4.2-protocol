#include "Client/Back/FrontManager.hpp"

#include "Client/Common/IPC/Header.hpp"
#include "Client/Common/IPC/Helper.hpp"

#include "Common/Log/Manager.hpp"
#include "Common/Network/Selector.hpp"

FrontManager &FrontManager::Instance()
{
    static FrontManager instance{};

    return instance;
}

bool FrontManager::wait_frontend()
{
    if (!m_acceptor.listen("/tmp/fix-backend.socket")) {
        Logger->log<logger::Level::Error>("Acceptor failed to listen, error: ", strerror(errno));
        return false;
    }
    Logger->log<logger::Level::Info>("Waiting connection of client on socket: /tmp/fix.backend.socket");
    m_socket = m_acceptor.accept();
    if (m_socket == nullptr) {
        Logger->log<logger::Level::Error>("Acceptor failed to accept, error: ", strerror(errno));
        return false;
    }
    Logger->log<logger::Level::Info>("Client front connected to server");
    m_thread = std::jthread(&FrontManager::receiveLoop, this);
    return true;
}

void FrontManager::notify(const net::Buffer &_buffer)
{
    m_socket->send(_buffer.data(), _buffer.size());
}

ts::Queue<net::Buffer> &FrontManager::getMessageQueue()
{
    return m_output;
}

FrontManager::FrontManager()
    : Logger(logger::Manager::newLogger("FrontManager"))
{
}

void FrontManager::receiveLoop(std::stop_token _st)
{
    Logger->log<logger::Level::Info>("Receiving loop started");
    net::Selector<net::UnixStream> selector;

    selector.timeout(1000);
    selector.client(m_socket);

    while (!_st.stop_requested()) {
        std::vector<net::Selector<net::UnixStream>::Client> clients = selector.pull();

        if (!clients.empty()) {
            int error = 0;
            std::vector<std::byte> bytes = clients[0]->receive(sizeof(ipc::Header), error);

            if (error != sizeof(ipc::Header)) {
                if (error == -1)
                    Logger->log<logger::Level::Error>("Error when receivin data from back: ", strerror(errno));
                else if (error == 0)
                    Logger->log<logger::Level::Fatal>("Frontend disconnected");
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

            Logger->log<logger::Level::Debug>("Received new data from frontend");
            m_output.push(std::move(buffer));
        }
    }
}