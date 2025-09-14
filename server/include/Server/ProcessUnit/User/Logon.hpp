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
    class LogonHandler : public AInputProcess<Context<data::UnparsedMessage>>
    {
        public:
            LogonHandler(StringOutputQueue &_tcp_output);
            virtual ~LogonHandler() = default;

        protected:
            void onInput(InputType _input) final;

        private:
            /// @brief Parse the message from the data available in the input
            /// @param _input Input data containing unparsed message info
            /// @return Return the parsed `Logon` message parsed on success, otherwise send a `SessionReject` and return `nullopt`
            std::optional<fix42::msg::Logon> parseMessage(InputType _input);

            /// @brief Temporary method to verify if the encryption method is None
            ///     todo remove this method when the encryption method is supported
            /// @param _logon Logon message currently processing
            /// @param _input Input processed
            /// @return `true` if the encprytion method is `None`, otherwise send a `SessionReject` message and return `false`
            bool verifyMessage(const fix42::msg::Logon &_logon, const InputType &_input);

            StringOutputQueue &m_tcp_output;

            ThreadPool<PU_LOGON_TP_SIZE> m_tp;
    };
}