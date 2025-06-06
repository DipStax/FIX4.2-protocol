#include <future>

#include "Server/Core/ProcessUnit/Network/TCPInput.hpp"

#include "Common/Message/Message.hpp"
#include "Common/Log/Manager.hpp"

namespace pu
{
    TCPInputNetwork::TCPInputNetwork(InputRouter &_output, InputNetworkOutput &_error, uint32_t _port)
        : m_output(_output), m_error(_error), Logger(log::Manager::newLogger("TCP-Input"))
    {
        (void)m_acceptor.listen(_port);
        (void)m_acceptor.setBlocking(false);
        m_selector.timeout(100);
        Logger->log<log::Level::Info>("[InNetwork] listening to port: ", _port);
        ClientStore::OnRemoveClient([this] (const ClientStore::Client _client) {
            m_selector.erase(_client->getSocket());
        });
    }

    void TCPInputNetwork::runtime(std::stop_token _st)
    {
        Logger->log<log::Level::Info>("Starting process unit...");
        ClientStore::ClientSocket accept = nullptr;
        std::vector<ClientStore::ClientSocket> clients;

        while (!_st.stop_requested()) {
            accept = m_acceptor.accept();
            if (accept) {
                ClientStore::Instance().newClientSocket(accept);
                m_selector.client(accept);
                Logger->log<log::Level::Info>("[InNetwork] Accepted new client: "); // todo log
            }
            clients = m_selector.pull();
            if (clients.size())
                Logger->log<log::Level::Debug>("[InNetwork] Received event from: ", clients.size(), " clients");
            for (ClientStore::ClientSocket &_client : clients) {
                ClientStore::Client client = ClientStore::Instance().findClient(_client);

                if (client == nullptr) {
                    m_selector.erase(_client);
                    (void)_client->close();
                    Logger->log<log::Level::Warning>("[InNetwork] Unable to find the client's information: ");
                } else if (process(client)) {
                    Logger->log<log::Level::Info>("[InNetwork] Disconnecting client (", *client, ")");
                    ClientStore::Instance().removeClient(client);
                }
            }
        }
    }

    bool TCPInputNetwork::process(const ClientStore::Client &_client)
    {
        int error = 0;
        fix::Serializer::AnonMessage msg;
        fix::Reject reject;

        if (!_client->getSocket()) { // todo is_open
            Logger->log<log::Level::Info>("Client disconnected");
            return true;
        }
        std::string data(_client->getSocket()->receive(MAX_RECV_SIZE, error));

        Logger->log<log::Level::Info>("Received from the client: (", _client->getUserId(), "), data:", data);
        if (error == 0) {
            Logger->log<log::Level::Error>("no data receive from the client: ");
            return true;
        }
        if (fix::Serializer::run(data, msg) != fix::Serializer::Error::None) {
            Logger->log<log::Level::Error>("will parsing the client message");
            // build reject
            m_error.append(_client, std::chrono::system_clock::now(), std::move(reject));
            return false;
        }
        m_output.append(_client, std::chrono::system_clock::now(), std::move(msg));
        return false;
    }
}