#include "Client/Shared/IPC/IPCNetworkManager.hpp"
#include "Client/Shared/IPC/Header.hpp"

#include "Shared/Log/Manager.hpp"
#include "Shared/Network/Selector.hpp"

template<class SocketType>
void IPCNetworkManager<SocketType>::send(const net::Buffer &_buffer)
{
    m_socket->send(_buffer.data(), _buffer.size());
}

template<class SocketType>
IPCNetworkManager<SocketType>::IPCNetworkManager(const std::string &_name)
    : Logger(logger::Manager::newLogger(_name))
{
}

template<class SocketType>
void IPCNetworkManager<SocketType>::receiveLoop(std::stop_token _st)
{
    Logger->log<logger::Level::Info>("Receiving loop started");
    net::Selector<SocketType> selector;

    selector.timeout(1000);
    selector.client(m_socket);

    while (!_st.stop_requested()) {
        std::vector<typename net::Selector<SocketType>::Client> clients = selector.pull();

        if (!clients.empty()) {
            int error = 0;
            std::vector<std::byte> bytes = clients[0]->receive(sizeof(ipc::Header), error);

            if (error != sizeof(ipc::Header)) {
                if (error == -1)
                    onError(errno);
                else if (error == 0)
                    onDisconnection();
                else
                    onWrongSize(bytes, error);
                continue;
            }
            net::Buffer buffer;
            ipc::Header header;

            buffer.append(bytes.data(), bytes.size());
            buffer >> header;
            bytes = clients[0]->receive(header.BodySize, error);
            if (error != static_cast<int>(header.BodySize)) {
                onWrongBodySize(bytes, header.BodySize);
                continue;
            }
            buffer.append(bytes.data(), bytes.size());
            buffer.reset();

            Logger->log<logger::Level::Debug>("Received new data from initator");
            onReceive(buffer);
        }
    }
}