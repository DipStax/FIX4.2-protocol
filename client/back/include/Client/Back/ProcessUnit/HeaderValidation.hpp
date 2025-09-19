#pragma once

#include "Client/Back/ProcessUnit/data/Global.hpp"

#include "Shared/ProcessUnit/AInputProcess.hpp"

namespace pu
{
    /// @brief Process Unit verifying the content of each header
    class HeaderValidation : public AInputProcess<Context<data::UnparsedMessage>>
    {
        public:
            /// @brief Construct the default implementation of the network output unit
            /// @param _output Standard message output
            /// @param _error Error messsage output
            HeaderValidation(UnparsedMessageQueue &_output, StringOutputQueue &_error);
            virtual ~HeaderValidation() = default;

        protected:
            /// @brief Verify all the value contained in the header
            /// @param _input Input to process
            void onInput(InputType _input) final;

            /// @brief Verify value of the header link to the positional tag
            /// @param _header Header to verify
            /// @return Return a ready to send `SessionReject` if `BeginString` or `MsgType` tag have invalid value
            [[nodiscard]] std::optional<fix42::msg::SessionReject> verifyPositionValue(const fix42::Header &_header);
            /// @brief Verify value of the header related to the user if login
            /// @param _header Header to verify
            /// @return Return a ready to send `SessionReject` if `TargetCompId`, `SenderCompId` or `MsgSeqNum` tag have invalid value
            [[nodiscard]] std::optional<fix42::msg::SessionReject> verifyUserSpecific(const fix42::Header &_header);

        private:
            UnparsedMessageQueue &m_output;             ///< Standard output for incoming message
            StringOutputQueue &m_error;                 ///< Error output to TCP/IP network output
    };
}