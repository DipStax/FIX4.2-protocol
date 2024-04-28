#include <future>

#include "Server/Core/Pipeline/InNetwork.hpp"
#include "Common/Core/Logger.hpp"
#include "Common/Message/Message.hpp"

namespace pip
{
    template<class Func>
    requires IsProcessor<Func, ClientSocket &, InAction &, InOutNetwork &>
    InNetwork<Func>::InNetwork(std::vector<ClientSocket> &_clients, InAction &_output, InOutNetwork &_error, uint32_t _port)
        : m_clients(_clients), m_output(_output), m_error(_error), m_acceptor(), m_selector()
    {
        (void)m_acceptor.listen(_port);
        (void)m_acceptor.blocking(false);
        m_selector.timeout(100);
        Logger::Log("[InNetwork] listening to port: ", _port);
    }

    template<class Func>
    requires IsProcessor<Func, ClientSocket &, InAction &, InOutNetwork &>
    InNetwork<Func>::~InNetwork()
    {
        (void)this->template stop();
    }

    template<class Func>
    requires IsProcessor<Func, ClientSocket &, InAction &, InOutNetwork &>
    bool InNetwork<Func>::start()
    {
        if (!this->m_running)
            this->template tstart(this);
        Logger::Log("[InNetwork] Running: ", this->m_running);
        return this->m_running;
    }

    template<class Func>
    requires IsProcessor<Func, ClientSocket &, InAction &, InOutNetwork &>
    void InNetwork<Func>::loop()
    {
        Logger::SetThreadName(THIS_THREAD_ID, "Network Input");

        Client accept = nullptr;
        std::vector<Client> clients;

        while (this->m_running) {
            accept = m_acceptor.accept();
            if (accept) {
                m_clients.emplace_back(accept);
                m_selector.client(accept);
                Logger::Log("[InNetwork] Accepted new client: "); // todo log
            }
            clients = m_selector.pull();
            if (clients.size())
                Logger::Log("[InNetwork] Received event from: ", clients.size(), " clients");
            for (Client &_client : clients) {
                auto client = std::find_if(m_clients.begin(), m_clients.end(), [_client] (const ClientSocket _lclient) {
                    return _client == _lclient.getSocket();
                });
                if (client == m_clients.end()) {
                    fix::Reject reject;

                    (void)_client->close();
                    Logger::Log("[InNetwork] Unable to find the client's information: "); // todo log
                    // build reject
                    m_error.append(ClientSocket(_client), std::move(reject));
                    continue;
                } else if (Func::run(*client, m_output, m_error)) {
                    Logger::Log("[InNetwork] Disconnecting client: "); // todo log
                    m_selector.erase(client->getSocket());
                    m_clients.erase(client);
                }
            }
        }
    }
}