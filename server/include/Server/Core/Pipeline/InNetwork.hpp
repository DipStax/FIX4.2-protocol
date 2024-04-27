#pragma once

#include "Common/Network/Selector.hpp"
#include "Server/Core/Pipeline/Core.hpp"
#include "Server/Core/Pipeline/Naming.hpp"
#include "Server/Core/meta.hpp"
#include "Server/Network/Processor.hpp"

#ifndef NET_RECV_SIZE
    #define NET_RECV_SIZE 4096
#endif

namespace pip
{
    /// @brief Pipeline managing the Network input.
    /// @tparam T is the socket type managed.
    /// @tparam _T is function to process when an action is needed on a socket.
    /// @tparam __T is the format socket are stored in the vector referenced.
    template<IsSocket T, class _T, class __T = std::shared_ptr<T>>
    requires SocketClient<__T, T> && IsProcessor<_T, ClientSocket &, InAction &, InOutNetwork &>
    class InNetwork : public Pipeline<InNetwork<T, _T, __T>>
    {
        public:
            /// @brief Socket format used
            using Client = net::Acceptor<T>::Client;

            /// @brief Core pipeline type
            using PipeType = Pipeline<InNetwork<T, _T, __T>>;

            /// @brief Setup the network input and start listening on _port
            /// @param _clients List of clients connected in to this network
            /// @param _output Output data queue of the pipeline.
            /// @param _error Error output directly connected to pip::OutNetwork
            /// @param _port Port on which the network will listen to, for connection and receive message.
            InNetwork(std::vector<__T> &_clients, InAction &_output, InOutNetwork &_error, uint32_t _port);
            /// @brief Disconnect from listening, stop the pipeline and the destroy it.
            ~InNetwork();

            /// @brief Run the pipeline
            /// @return Return true if the pipeline as correctly started else false.
            [[nodiscard]] bool start();

            /// @brief Core function of the pipeline determining it's behavior
            void loop();

        private:
            std::vector<__T> &m_clients;            ///< Client list
            InAction &m_output;                     ///< Ouput data queue
            InOutNetwork &m_error;                  ///< Output message queue directly to the pip::OutNetwork

            net::Acceptor<T> m_acceptor;            ///< Acceptor listening the the port passed as parameter.
            net::Selector<T> m_selector;            ///< Selector managing the client list action.
    };
}

#include "Server/Core/Pipeline/InNetwork.inl"
