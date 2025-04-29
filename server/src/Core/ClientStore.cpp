#include <algorithm>
#include <memory>

#include "Server/Core/ClientStore.hpp"

ClientStore &ClientStore::Instance()
{
    static ClientStore instance{};

    return instance;
}

void ClientStore::newClientSocket(ClientSocket _client)
{
    std::unique_lock lock(m_mutex);

    m_clients.emplace_back(std::make_shared<InternalClient>(_client));
}

ClientStore::Client ClientStore::findClient(ClientSocket _client)
{
    std::shared_lock lock(m_mutex);

    auto it = std::find_if(m_clients.begin(), m_clients.end(), [_client] (const Client _original) {
        return _client == _original->getSocket();
    });
    return it == m_clients.end() ? nullptr : *it;
}

ClientStore::Client ClientStore::findClient(const std::string &_client_id)
{
    std::shared_lock lock(m_mutex);

    auto it = std::find_if(m_clients.begin(), m_clients.end(), [_client_id] (const Client _original) {
        return _client_id == _original->getUserId();
    });
    return it == m_clients.end() ? nullptr : *it;
}

void ClientStore::removeClient(Client _client)
{
    std::unique_lock lock(m_mutex);

    m_clients.erase(
        std::remove_if(m_clients.begin(), m_clients.end(), [_client] (Client _original) {
            return _client == _original;
        }),
        m_clients.end()
    );
}

void ClientStore::Apply(ForEachCallback<void> _callback)
{
    std::shared_lock lock(m_mutex);

    for (Client _client : m_clients)
        _callback(_client);
}