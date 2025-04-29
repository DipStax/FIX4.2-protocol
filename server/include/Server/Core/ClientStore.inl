#include "Server/Core/ClientStore.hpp"

template<class T>
std::vector<T> ClientStore::forEach(ForEachCallback<T> _callback)
{
    std::shared_lock lock(m_mutex);
    std::vector<T> result;

    result.reserve(m_clients.size());
    for (Client _client : m_clients)
        result.push_back(_callback(_client));
}