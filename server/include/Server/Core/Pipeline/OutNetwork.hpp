#pragma once

#include "Server/Core/meta.hpp"
#include "Common/Thread/Pool.hpp"
#include "Server/Core/Pipeline/Naming.hpp"

#ifndef TS_SIZE_ON
    #define TS_SIZE_ON 1
#endif

namespace pip
{
    /// @brief Pipeline to send reply to a client over TCP network.
    template<class Func>
    requires IsProcessor<Func, OutNetworkInput &, std::vector<ClientSocket> &>
    class OutNetwork : public IProcessUnit
    {
        public:
            /// @brief Socket format used
            using Client = net::Acceptor<net::tcp::Socket>::Client;

            /// @brief Construct the pipeline.
            /// @param _input Input data queue.
            /// @param _clients List of the connected client.
            OutNetwork(std::vector<ClientSocket> &_clients, InOutNetwork &_input);
            /// @brief Stop and then destroy the pipeline.
            virtual ~OutNetwork() = default;

        protected:
            void runtime(std::stop_token _st);

        private:
            InOutNetwork &m_input;                       ///< Intput data queue.
            std::vector<ClientSocket> &m_clients;       ///< List of connected clients.

            ThreadPool<TS_SIZE_ON> m_tp;        ///< Thread pool used to send data to the target client in async.
    };
}

#include "Server/Core/Pipeline/OutNetwork.inl"