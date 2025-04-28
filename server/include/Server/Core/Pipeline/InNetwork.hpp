#pragma once

#include "Common/Network/Selector.hpp"
#include "Server/Core/Pipeline/Naming.hpp"
#include "Server/Core/meta.hpp"
#include "Server/Core/Pipeline/IProcessUnit.hpp"

#ifndef NET_RECV_SIZE
    #define NET_RECV_SIZE 4096
#endif

namespace pip
{
    /// @brief Pipeline managing the Network input.
    /// @tparam T is the socket type managed.
    /// @tparam _T is function to process when an action is needed on a socket.
    /// @tparam __T is the format socket are stored in the vector referenced.
    template<class Func>
    requires IsProcessor<Func, ClientSocket &, InAction &, InOutNetwork &>
    class InNetwork : public IProcessUnit
    {
        public:
            /// @brief Socket format used
            using Client = net::Acceptor<net::tcp::Socket>::Client;

            /// @brief Setup the network input and start listening on _port
            /// @param _clients List of clients connected in to this network
            /// @param _output Output data queue of the pipeline.
            /// @param _error Error output directly connected to pip::OutNetwork
            /// @param _port Port on which the network will listen to, for connection and receive message.
            InNetwork(std::vector<ClientSocket> &_clients, InAction &_output, InOutNetwork &_error, uint32_t _port);
            /// @brief Disconnect from listening, stop the pipeline and the destroy it.
            virtual ~InNetwork() = default;

        protected:
            void runtime(std::stop_token _st);

        private:
            std::vector<ClientSocket> &m_clients;            ///< Client list
            InAction &m_output;                     ///< Ouput data queue
            InOutNetwork &m_error;                  ///< Output message queue directly to the pip::OutNetwork

            net::Acceptor<net::tcp::Socket> m_acceptor;            ///< Acceptor listening the the port passed as parameter.
            net::Selector<net::tcp::Socket> m_selector;            ///< Selector managing the client list action.
    };
}

#include "Server/Core/Pipeline/InNetwork.inl"
