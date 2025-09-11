#pragma once

#include "Shared/Message/Fix.hpp"

namespace fix
{
    /// @brief Fix [Business Message Reject](https://www.onixs.biz/fix-dictionary/4.2/msgType_j_106.html) message class.
    class BusinessMessageReject : public old_Message
    {
        public:
            BusinessMessageReject();
            ~BusinessMessageReject() = default;

            /// @brief Set the reference to the sequence number [45](https://www.onixs.biz/fix-dictionary/4.2/tagNum_45.html).
            /// @param _val Value assigned to it: int.
            void set45_refSeqNum(const std::string &_val);

            /// @brief Set an optional text to explain the reason [58](https://www.onixs.biz/fix-dictionary/4.2/tagNum_58.html).
            /// @param _val Value assigned to it: string.
            void set58_text(const std::string &_val);

            /// @brief Reference to the fix message type [372](https://www.onixs.biz/fix-dictionary/4.2/tagNum_372.html).
            /// @param _val Value assigned to it: char.
            void set372_refMsgType(const std::string &_val);

            /// @brief Reference buisness ID of the reject [379](https://www.onixs.biz/fix-dictionary/4.2/tagNum_379.html).
            /// @param _val Value assing to it: string.
            void set379_businessRejectRefId(const std::string &_val);

            /// @brief Reference buisness reject reason [380](https://www.onixs.biz/fix-dictionary/4.2/tagNum_380.html).
            /// @param _val Value assing to it: string.
            void set380_businessRejectReason(const std::string &_val);

            static constexpr const char MsgType[] = "j";        ///< Message type value as string.
            static constexpr const char cMsgType = MsgType[0];  ///< Message type value as char.

            static constexpr const char Other[] = "0";
            static constexpr const char UnknowId[] = "1";
            static constexpr const char UnknowSecurity[] = "2";
            static constexpr const char UnsupportedMsgType[] = "3";
            static constexpr const char AppNotAvailable[] = "4";
            static constexpr const char ConditionalReqFielMissing[] = "5";
    };
}
