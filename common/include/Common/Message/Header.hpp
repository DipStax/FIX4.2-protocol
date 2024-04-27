#pragma once

#include <string>
#include "Common/Message/Serializer.hpp"
#include "Common/Core/Order.hpp"

namespace fix
{
    class Reject;

    /// @brief Fix [Header](https://www.onixs.biz/fix-dictionary/4.2/compBlock_StandardHeader.html) class.
    class Header
    {
        public:
            Header() = default;
            ~Header() = default;

            /// @brief Verify if the header receive is correctly formated.
            /// @param _msg Message to check.
            /// @return If the first element is true then second is set, otherwise it rigly formated.
            static std::pair<bool, Reject> Verify(Serializer::AnonMessage &_msg, const UserId &_sender, const UserId &_target, size_t _seqnum);

            /// @brief Set the body length of the message [9](https://www.onixs.biz/fix-dictionary/4.2/tagNum_9.html).
            /// @param _val Value assigned to it: int.
            void set9_bodyLength(const std::string &_len);
            [[deprecated("Use the same function with a string")]]
            void set9_bodyLength(const size_t &_len);

            /// @brief Set the type of message send [35](https://www.onixs.biz/fix-dictionary/4.2/tagNum_35.html).
            /// @param _val Value assigned to it: string.
            void set35_MsgType(const std::string &_val);

            /// @brief Set the sequence number of the message [34](https://www.onixs.biz/fix-dictionary/4.2/tagNum_34.html).
            /// @param _val Value assigned to it: int.
            void set34_msgSeqNum(const std::string &_val);
            /// @brief Set the sequence number of the message [34](https://www.onixs.biz/fix-dictionary/4.2/tagNum_34.html).
            /// @param _val Value assigned to it: int.
            [[deprecated("Use the same function with a string")]]
            void set34_msgSeqNum(const size_t &_val);

            /// @brief Set the sender Id of the message [49](https://www.onixs.biz/fix-dictionary/4.2/tagNum_49.html).
            /// @param _val Value assigned to it: string.
            void set49_SenderCompId(const std::string &_val);

            /// @brief Set the receiver Id of the message [56](https://www.onixs.biz/fix-dictionary/4.2/tagNum_56.html).
            /// @param _val Value assigned to it: string.
            void set56_TargetCompId(const std::string &_val);
            [[nodiscard]] uint64_t getTargetCompId() const;

            /// @brief Convert the header to a string.
            operator std::string() const;

        protected:
            friend class Message;

            void setSendingTime();
            [[deprecated("Use set34_msgSeqNum() before calling operator std::string instead")]]
            void updateMsgSeqNum();

            /// @brief Get the partial header for the check sum.
            /// @return String formated partial header.
            std::string getPartialHeader() const;

        private:
            std::string BeginString = "FIX.4.2";    // 8 --> Init at the beginning of the session
            std::string BodyLength = "0";           // 9
            std::string MsgType{};                  // 35
            std::string SenderCompId{};             // 49 --> Init at the beginning of the session
            std::string TargetCompId{};             // 56 --> Init at the beginning of the session
            std::string MsgSeqNum = "0";            // 34
            std::string SendingTime{};              // 56
    };
}