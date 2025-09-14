#pragma once

#include "Server/ProcessUnit/data/Global.hpp"

#include "Shared/Message-v2/Message.hpp"
#include "Shared/ProcessUnit/AInputProcess.hpp"
#include "Shared/Thread/Pool.hpp"
#include "Shared/Log/ILogger.hpp"

#ifndef PU_LOGON_TP_SIZE
    #define PU_LOGON_TP_SIZE 1
#endif

namespace pu::user
{
    /// @brief Process Unit dedicated to processing `Logon` message
    class LogonHandler : public AInputProcess<Context<data::UnparsedMessage>>
    {
        public:
            /// @brief Default constructor for the `Logon` message handler
            /// @param _tcp_output Message queue to the TCP/IP output
            LogonHandler(StringOutputQueue &_tcp_output);
            virtual ~LogonHandler() = default;

        protected:
            /// @brief Process in a thread pool every `Logon` message
            /// @param _input Input containing the `Logon` message
            void onInput(InputType _input) final;

        private:
            /// @brief Temporary method to verify if the encryption method is None
            ///     todo remove this method when the encryption method is supported
            /// @param _logon Logon message currently processing
            /// @param _input Input processed
            /// @return `true` if the encprytion method is `None`, otherwise send a `SessionReject` message and return `false`
            bool verifyMessage(const fix42::msg::Logon &_logon, const InputType &_input);

            StringOutputQueue &m_tcp_output;    ///< TCP/IP ouput queue

            ThreadPool<PU_LOGON_TP_SIZE> m_tp;  ///< Thread pool that process all the incoming message
    };
}