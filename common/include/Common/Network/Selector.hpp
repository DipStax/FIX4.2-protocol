#pragma once

#include <map>
#include <memory>

#include "Common/Network/Acceptor.hpp"
#include "Common/Network/CEPoll.hpp"

#ifndef MAX_EVENT_EPOLL
    #define MAX_EVENT_EPOLL 30
#endif

#ifndef MAX_SOCKET
    #define MAX_SOCKET 30
#endif

namespace net
{
    /// @brief Class implementing the c::EPoll to select socket with pending event.
    /// @tparam T is the of socket supported.
    template<IsSocket T>
    class Selector : public c::EPoll
    {
        public:
            /// @brief Type of client.
            using Client = Acceptor<T>::Client;

            Selector();
            ~Selector() = default;

            /// @brief Add a client to the tracked socket.
            /// @param _client New client to track event on.
            /// @return False if the client is already track by the selector or an error occured, otherwise true.
            bool client(Client _client);
            void erase(Client _client);


            /// @brief Set the timeout for event checking in the function net::Selector<T>::pull.
            /// @param _to Time in second.
            void timeout(float _to);
            /// @brief Get the actual timeout of the event checking of the function net::Selector<T>::pull.
            /// @return Timeout in second.
            [[nodiscard]] float timeout() const;

            /// @brief Fetch all the client where event is pending.
            /// @return Client list waiting to be treated.
            std::vector<Client> pull();

            /// @brief Get the number of client the selector track.
            /// @return Number of clients.
            [[nodiscard]] size_t size() const;

        private:
            std::map<int, Client> m_clients;    ///< Number of client, with the key as the file descriptor of the client.

            float m_to;                         ///< Timeout of the function net::Selector<T>::pull.
    };
}

#include "Common/Network/Selector.inl"