#pragma once

#include "Common/Message/Fix.hpp"

namespace fix
{
    /// @brief Fix [Reject](https://www.onixs.biz/fix-dictionary/4.2/msgType_3_3.html) message class.
    class Reject : public Message
    {
        public:
            Reject();
            ~Reject() = default;

            /// @brief Reference the sequence number of the rejected fix message [45](https://www.onixs.biz/fix-dictionary/4.2/tagNum_45.html).
            /// @param _val Value assigned to it: int.
            void set45_refSeqNum(const std::string &_val);

            /// @brief Set an optional text to explain the reason [58](https://www.onixs.biz/fix-dictionary/4.2/tagNum_58.html).
            /// @param _val Value assigned to it: string.
            void set58_text(const std::string &_val);

            /// @brief Reference to the tag Id [371](https://www.onixs.biz/fix-dictionary/4.2/tagNum_371.html).
            /// @param _val Value assigned to it: char.
            void set371_refTagId(const std::string &_val);

            /// @brief Reference to the [fix message type](https://www.onixs.biz/fix-dictionary/4.2/tagNum_35.html) [372](https://www.onixs.biz/fix-dictionary/4.2/tagNum_371.html).
            /// @param _val Value assigned to it: char.
            void set372_refMsgType(const std::string &_val);

            /// @brief Set the reason why the fix message got rejected [373](https://www.onixs.biz/fix-dictionary/4.2/tagNum_373.html).
            /// @param _val Value assigned to it: int.
            void set373_sessionRejectReason(const std::string &_val);

            static constexpr const char InvalidTag[] = "0";      ///< Reason of reject when a tag is invalid.
            static constexpr const char ReqTagMissing[] = "1";   ///< Reason of reject when a required tag is missing.
            static constexpr const char NotSupporType[] = "2";   ///< Reason of reject when a tag is not supported by the message.
            static constexpr const char UndefineTag[] = "3";     ///< Reason of reject when a tag is unknow.
            static constexpr const char EmptyValue[] = "4";      ///< Reason of reject when a tag's value is empty.
            static constexpr const char ValueOORange[] = "5";    ///< Reason of reject when a tag's value is out of range.
            static constexpr const char IncorrectFormat[] = "6"; ///< Reason of reject when a tag's value is incorrectly formated.
            static constexpr const char DecryptionIssue[] = "7"; ///< Reason of reject when their a decryption issue

            static constexpr const char MsgType[] = "3";         ///< Message type value as string.
            static constexpr const char cMsgType = MsgType[0];  ///< Message type value as char
    };
}