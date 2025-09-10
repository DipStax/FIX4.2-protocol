#include <future>

#include "Server/ProcessUnit/Network/TCPInput.hpp"

#include "Shared/Message/Message.hpp"
#include "Shared/Log/Manager.hpp"

namespace pu
{
    TCPInputNetwork::TCPInputNetwork(InputRouter &_output, InputNetworkOutput &_error, uint32_t _port)
        : AProcessUnitBase("Server/NET/TCP-Input"),
        m_output(_output), m_error(_error)
    {
        (void)m_acceptor.listen(_port);
        (void)m_acceptor.setBlocking(false);
        m_selector.timeout(100);
        Logger->log<logger::Level::Debug>("listening to port: ", _port);

        ClientStore::OnRemoveClient([this] (const ClientStore::Client _client) {
            Logger->log<logger::Level::Info>("Removing client from selector (", *_client, ")");
            if (!_client->getSocket()->close())
                Logger->log<logger::Level::Warning>("Closing socket for client failed");
            m_selector.erase(_client->getSocket());
        });
    }

    void TCPInputNetwork::runtime(std::stop_token _st)
    {
        Logger->log<logger::Level::Info>("Launching process unit runtime");

        ClientStore::ClientSocket accept = nullptr;
        std::vector<ClientStore::ClientSocket> clients;

        while (!_st.stop_requested()) {
            accept = m_acceptor.accept();
            if (accept) {
                ClientStore::Instance().newClientSocket(accept);
                m_selector.client(accept);
                Logger->log<logger::Level::Info>("[InNetwork] Accepted new client: "); // todo log
            }
            clients = m_selector.pull();
            if (clients.size())
                Logger->log<logger::Level::Debug>("[InNetwork] Received event from: ", clients.size(), " clients");
            for (ClientStore::ClientSocket &_client : clients) {
                ClientStore::Client client = ClientStore::Instance().findClient(_client);

                if (client == nullptr) {
                    m_selector.erase(_client);
                    (void)_client->close();
                    Logger->log<logger::Level::Warning>("[InNetwork] Unable to find the client's information: ");
                } else if (process(client)) {
                    Logger->log<logger::Level::Info>("[InNetwork] Disconnecting client (", *client, ")");
                    ClientStore::Instance().removeClient(client);
                }
            }
        }
        Logger->log<logger::Level::Info>("Exiting process unit runtime");
    }

    bool TCPInputNetwork::process(const ClientStore::Client &_client)
    {
        int error = 0;
        fix::Serializer::AnonMessage msg;
        fix::Reject reject;

        if (_client->getSocket() == nullptr || !_client->getSocket()->isOpen()) {
            Logger->log<logger::Level::Info>("Client socket not found or closed");
            return true;
        }
        std::vector<std::byte> bytes = _client->getSocket()->receive(2048, error);
        std::string data(reinterpret_cast<const char*>(bytes.data()), + bytes.size());

        Logger->log<logger::Level::Info>("Received from the client: (", _client->getUserId(), "), data:", data);
        if (error == 0) {
            Logger->log<logger::Level::Error>("no data receive from the client: ");
            return true;
        }
        if (fix::Serializer::run(data, msg) != fix::Serializer::Error::None) {
            Logger->log<logger::Level::Error>("will parsing the client message");
            // build reject
            m_error.append(_client, std::chrono::system_clock::now(), std::move(reject));
            return false;
        }
        m_output.append(_client, std::chrono::system_clock::now(), std::move(msg));
        return false;
    }
}