#include <algorithm>
#include <memory>

#include "Server/Core/ClientStore.hpp"

#include "Common/Log/Manager.hpp"

ClientStore &ClientStore::Instance()
{
    static ClientStore instance{};

    return instance;
}

void ClientStore::newClientSocket(ClientSocket _client)
{
    Client new_client;

    {
        std::unique_lock lock_client(m_client_mutex);

        m_clients.emplace_back(std::make_shared<InternalClient>(_client));
        new_client = m_clients.back();
    }

    m_tp_onnew.enqueue([this, new_client] () {
        std::shared_lock lock_onnew(m_onnew_mutex);

        for (OnClientCallback _cb : m_onnew)
            _cb(new_client);
    });
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
    Logger->log<log::Level::Info>("Request to remove client (", *_client, ")");
    _client->shouldDisconnect(true);
    _client->disconnect();
    m_tp_onremove.enqueue([this, _client] {
        {
            std::unique_lock lock_client(m_client_mutex);

            Logger->log<log::Level::Debug>("Removing client from client list (", *_client, ")");
            auto it = std::find_if(m_clients.begin(), m_clients.end(), [_client] (const Client _original) {
                return _client == _original;
            });
            m_clients.erase(it);
        }
        {
            std::shared_lock lock_remove(m_onremove_mutex);

            Logger->log<log::Level::Verbose>("Applying callback (", m_onremove.size(), ") to remove client (", *_client, ")");
            for (OnClientCallback _cb : m_onremove)
                _cb(_client);
        }
    });
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

ClientStore::ClientStore()
    : Logger(log::Manager::newLogger("ClientStore"))
{
}