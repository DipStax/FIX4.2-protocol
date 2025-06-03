#pragma once

#include <shared_mutex>
#include <functional>

#include "Server/Core/InternalClient.hpp"

#include "Common/Network/Acceptor.hpp"
#include "Common/Network/Socket.hpp"
#include "Common/Thread/Pool.hpp"

class ClientStore
{
    public:
        using ClientSocket = net::Acceptor<net::StreamTcp>::Client;
        using Client = std::shared_ptr<InternalClient>;

        template<class T>
        using ForEachCallback = std::function<T(Client)>;
        using OnClientCallback = std::function<void(const Client &)>;

        ~ClientStore() = default;

        static ClientStore &Instance();

        void newClientSocket(ClientSocket _client);
        static void OnNewClient(OnClientCallback _callback);

        Client findClient(ClientSocket _client);
        Client findClient(const std::string &_client_id);

        void removeClient(Client _client);
        static void OnRemoveClient(OnClientCallback _callback);

        template<class T>
        std::vector<T> forEach(ForEachCallback<T> _callback);

        void Apply(ForEachCallback<void> _callback);

    private:
        ClientStore() = default;

        std::shared_mutex m_client_mutex{};
        std::vector<Client> m_clients{};

        static inline std::shared_mutex m_onnew_mutex{};
        static inline std::vector<OnClientCallback> m_onnew{};
        static inline ThreadPool<1> m_tp_onnew{};

        static inline std::shared_mutex m_onremove_mutex{};
        static inline std::vector<OnClientCallback> m_onremove{};
        static inline ThreadPool<1> m_tp_onremove{};
};

#include "Server/Core/ClientStore.inl"