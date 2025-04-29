#pragma once

#include <shared_mutex>
#include <functional>

#include "Server/Core/InternalClient.hpp"
#include "Common/Network/Acceptor.hpp"
#include "Common/Network/Socket.hpp"

class ClientStore
{
    public:
        using ClientSocket = net::Acceptor<net::tcp::Socket>::Client;
        using Client = std::shared_ptr<InternalClient>;

        template<class T>
        using ForEachCallback = std::function<T(Client _client)>;

        ~ClientStore() = default;

        static ClientStore &Instance();

        void newClientSocket(ClientSocket _client);

        Client findClient(ClientSocket _client);
        Client findClient(const std::string &_client_id);

        void removeClient(Client _client);

        template<class T>
        std::vector<T> forEach(ForEachCallback<T> _callback);

        void Apply(ForEachCallback<void> _callback);

    private:
        ClientStore() = default;

        std::shared_mutex m_mutex{};
        std::vector<Client> m_clients{};
};

#include "Server/Core/ClientStore.inl"