#pragma once

#include "Server/Core/meta.hpp"
#include "Server/Core/ProcessUnit/Naming.hpp"

#include "Common/Thread/Pool.hpp"

#ifndef TS_SIZE_ON
    #define TS_SIZE_ON 10
#endif

namespace pu
{
    /// @brief Pipeline to send reply to a client over TCP network.
    template<class Func>
    requires IsProcessor<Func, Context<OutNetworkInput> &>
    class OutNetwork : public IProcessUnit<Context<OutNetworkInput>>
    {
        public:
            /// @brief Socket format used
            using Client = net::Acceptor<net::tcp::Socket>::Client;

            OutNetwork() = default;
            virtual ~OutNetwork() = default;

            [[nodiscard]] QueueInputType &getInput();

        protected:
            std::string getThreadName() const;

            void runtime(std::stop_token _st);

        private:
            QueueInputType m_input;                       ///< Intput data queue.

            ThreadPool<TS_SIZE_ON> m_tp;        ///< Thread pool used to send data to the target client in an other thread.
    };
}

#include "Server/Core/ProcessUnit/OutNetwork.inl"