#pragma once

#include "Server/ProcessUnit/data/Global.hpp"

#include "Shared/Message-v2/Message.hpp"
#include "Shared/ProcessUnit/AInputProcess.hpp"
#include "Shared/Thread/Pool.hpp"
#include "Shared/Log/ILogger.hpp"

#ifndef PU_LOGOUT_TP_SIZE
    #define PU_LOGOUT_TP_SIZE 1
#endif

namespace pu::user
{
    /// @brief Process Unit dedicated to processing `Logout` message
    class LogoutHandler : public AInputProcess<Context<data::UnparsedMessage>>
    {
        public:
            /// @brief Default constructor for the `Logout` message handler
            /// @param _tcp_output Message queue to the TCP/IP output
            LogoutHandler(StringOutputQueue &_tcp_output);
            virtual ~LogoutHandler() = default;

        protected:
            /// @brief Process in a thread pool every `Logout` message
            /// @param _input Input containing the `Logout` message
            void onInput(InputType _input) final;

        private:
            /// @brief Parse the message from the data available in the input
            /// @param _input Input data containing unparsed message info
            /// @return Return the parsed `Logout` message parsed on success, otherwise send a `SessionReject` and return `nullopt`
            std::optional<fix42::msg::Logout> parseMessage(InputType _input);

            StringOutputQueue &m_tcp_output;    ///< TCP/IP ouput queue

            ThreadPool<PU_LOGOUT_TP_SIZE> m_tp; ///< Thread pool that process all the incoming message
    };
}