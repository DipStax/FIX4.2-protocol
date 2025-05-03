#include <future>

#include "Server/Core/ProcessUnit/InNetwork.hpp"

#include "Common/Core/Logger.hpp"
#include "Common/Message/Message.hpp"

namespace pu
{
    template<class Func>
    requires IsProcessor<Func, const ClientStore::Client &, InputRouter &, InOutNetwork &>
    InNetwork<Func>::InNetwork(InputRouter &_output, InOutNetwork &_error, uint32_t _port)
        : m_output(_output), m_error(_error), m_acceptor(), m_selector()
    {
        (void)m_acceptor.listen(_port);
        (void)m_acceptor.blocking(false);
        m_selector.timeout(100);
        Logger::Log("[InNetwork] listening to port: ", _port);
    }

    template<class Func>
    requires IsProcessor<Func, const ClientStore::Client &, InputRouter &, InOutNetwork &>
    void InNetwork<Func>::runtime(std::stop_token _st)
    {
        ClientStore::ClientSocket accept = nullptr;
        std::vector<ClientStore::ClientSocket> clients;

        while (!_st.stop_requested()) {
            accept = m_acceptor.accept();
            if (accept) {
                ClientStore::Instance().newClientSocket(accept);
                m_selector.client(accept);
                Logger::Log("[InNetwork] Accepted new client: "); // todo log
            }
            clients = m_selector.pull();
            if (clients.size())
                Logger::Log("[InNetwork] Received event from: ", clients.size(), " clients");
            for (ClientStore::ClientSocket &_client : clients) {
                ClientStore::Client client = ClientStore::Instance().findClient(_client);

                if (client == nullptr) {
                    m_selector.erase(_client);
                    (void)_client->close();
                    Logger::Log("[InNetwork] Unable to find the client's information: ");
                } else if (Func::run(client, m_output, m_error)) {
                    Logger::Log("[InNetwork] Disconnecting client: ");
                    m_selector.erase(client->getSocket());
                    ClientStore::Instance().removeClient(client);
                }
            }
        }
    }
}