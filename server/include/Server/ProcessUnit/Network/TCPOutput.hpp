#pragma once

#include "Server/meta.hpp"
#include "Server/ProcessUnit/data/Global.hpp"

#include "Shared/ProcessUnit/AInputProcess.hpp"
#include "Shared/Thread/Pool.hpp"
#include "Shared/Log/ILogger.hpp"

#ifndef PU_NETOUT_TP_SIZE
    #define PU_NETOUT_TP_SIZE 10
#endif

namespace pu
{
    /// @brief Process Unit handling network TCP/IP output
    class TcpOutputNetwork : public AInputProcess<Context<data::StringOutput>>
    {
        public:
            using Client = net::Acceptor<net::INetTcp>::Client;

            TcpOutputNetwork();
            virtual ~TcpOutputNetwork() = default;

        protected:
            /// @brief Process the sending of message using a threadpool
            /// @param _input Input to process
            void onInput(InputType _input) final;

            /// @brief Add the check sum tag to the referenced message
            /// @param _msg Message to calculate and add the check sum tag
            static void AddCheckSum(std::string &_msg);

        private:
            ThreadPool<PU_NETOUT_TP_SIZE> m_tp; ///< Threadpool to process message sending
    };
}