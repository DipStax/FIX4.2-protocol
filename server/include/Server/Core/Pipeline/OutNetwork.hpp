#pragma once

#include "Server/Core/meta.hpp"
#include "Common/Thread/Pool.hpp"
#include "Server/Core/Pipeline/Core.hpp"
#include "Server/Core/Pipeline/Naming.hpp"

#ifndef TS_SIZE_ON
    #define TS_SIZE_ON 1
#endif

namespace pip
{
    /// @brief Pipeline to send reply to a client over TCP network.
    template<class Func>
    requires IsProcessor<Func, OutNetworkInput &, std::vector<ClientSocket> &>
    class OutNetwork : public Pipeline<OutNetwork<Func>>
    {
        public:
            /// @brief Socket format used
            using Client = net::Acceptor<net::tcp::Socket>::Client;

            /// @brief Core pipeline type.
            using PipeType = Pipeline<OutNetwork<Func>>;

            /// @brief Construct the pipeline.
            /// @param _input Input data queue.
            /// @param _clients List of the connected client.
            OutNetwork(std::vector<ClientSocket> &_clients, InOutNetwork &_input);
            /// @brief Stop and then destroy the pipeline.
            ~OutNetwork();

            /// @brief Run the pipeline
            /// @return Return true if the pipeline as correctly started else false.
            [[nodiscard]] bool start();

            /// @brief Core function of the pipeline determining it's behavior
            void loop();

        private:
            InOutNetwork &m_input;                       ///< Intput data queue.
            std::vector<ClientSocket> &m_clients;       ///< List of connected clients.

            ThreadPool<TS_SIZE_ON> m_tp;        ///< Thread pool used to send data to the target client in async.
    };
}

#include "Server/Core/Pipeline/OutNetwork.inl"