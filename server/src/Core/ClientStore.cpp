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
    std::vector<Client>::iterator it;

    {
        std::unique_lock lock_client(m_client_mutex);

        m_clients.emplace_back(std::make_shared<InternalClient>(_client));

        m_tp_onnew.enqueue([this, _client = m_clients.back()] () {
            std::shared_lock lock_onnew(m_onnew_mutex);

            for (OnClientCallback _cb : m_onnew)
                _cb(_client);
        });
    }
}

void ClientStore::OnNewClient(OnClientCallback _callback)
{
    std::unique_lock lock(m_onnew_mutex);

    m_onnew.push_back(_callback);
}

ClientStore::Client ClientStore::findClient(ClientSocket _client)
{
    std::shared_lock lock(m_client_mutex);

    auto it = std::find_if(m_clients.begin(), m_clients.end(), [_client] (const Client _original) {
        return _client == _original->getSocket();
    });
    return it == m_clients.end() ? nullptr : *it;
}

ClientStore::Client ClientStore::findClient(const std::string &_client_id)
{
    std::shared_lock lock(m_client_mutex);

    auto it = std::find_if(m_clients.begin(), m_clients.end(), [_client_id] (const Client _original) {
        return _client_id == _original->getUserId();
    });
    return it == m_clients.end() ? nullptr : *it;
}

void ClientStore::removeClient(Client _client)
{
    std::unique_lock lock_client(m_client_mutex);
    auto it = std::find_if(m_clients.begin(), m_clients.end(), [_client] (const Client _original) {
        return _client == _original;
    });

    m_tp_onremove.enqueue([this, _client] {
        std::shared_lock lock_remove(m_onremove_mutex);

        for (OnClientCallback _cb : m_onremove)
            _cb(_client);
    });
    m_clients.erase(it);
}

void ClientStore::OnRemoveClient(OnClientCallback _callback)
{
    std::unique_lock lock(m_onremove_mutex);

    m_onremove.push_back(_callback);
}

void ClientStore::Apply(ForEachCallback<void> _callback)
{
    std::shared_lock lock(m_client_mutex);

    for (Client _client : m_clients)
        _callback(_client);
}