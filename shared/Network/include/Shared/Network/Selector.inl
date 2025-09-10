#include <algorithm>
#include <iostream>

#include "Shared/Network/Selector.hpp"

namespace net
{
    template<IsSocketDomain T>
    Selector<T>::Selector()
        : c::EPoll((int)MAX_EVENT_EPOLL), m_to(0)
    {
    }

    template<IsSocketDomain T>
    bool Selector<T>::client(Client _client)
    {
        if (_client == nullptr)
            return false;

        Event event;
        int fd = _client->FD();

        event.data.fd = fd;
        event.events = EPOLLIN | EPOLLET;
        if (ctl(EPOLL_CTL_ADD, fd, &event) != -1) {
            m_clients.emplace(fd, _client);
            return true;
        }
        return false;
    }

    template<IsSocketDomain T>
    void Selector<T>::erase(Client _client)
    {
        if (_client == nullptr)
            return;
        std::erase_if(m_clients, [_client] (const std::pair<int, Client> &_lclient) {
            return _client->FD() == _lclient.second->FD();
        });
    }

    template<IsSocketDomain T>
    void Selector<T>::timeout(float _to)
    {
        m_to = _to;
    }

    template<IsSocketDomain T>
    float Selector<T>::timeout() const
    {
        return m_to;
    }

    template<IsSocketDomain T>
    std::vector<typename Selector<T>::Client> Selector<T>::pull()
    {
        Event events[MAX_EVENT_EPOLL];
        int set = wait(events, m_to);
        std::vector<Client> clients;

        if (set == -1)
            return clients;
        for (int it = 0; it < std::min(set, MAX_EVENT_EPOLL); it++) {
            clients.emplace_back(m_clients.at(events[it].data.fd));
        }
        return clients;
    }


    template<IsSocketDomain T>
    size_t Selector<T>::size() const
    {
        return m_clients.size();
    }

}