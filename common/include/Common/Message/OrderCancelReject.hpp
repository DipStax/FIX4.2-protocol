#pragma once

#include "Common/Message/Fix.hpp"

namespace fix
{
    /// @brief Fix [Order Cancel Reject](https://www.onixs.biz/fix-dictionary/4.2/msgType_9_9.html) class.
    class OrderCancelReject: public Message
    {
        public:
            OrderCancelReject();
            ~OrderCancelReject() = default;

            /// @brief Set the order Id [11](https://www.onixs.biz/fix-dictionary/4.2/tagNum_11.html).
            /// @param _val Value assigned to it: string.
            void set11_clOrdID(const std::string &_val);

            /// @brief Set the reference order Id of the report [37](https://www.onixs.biz/fix-dictionary/4.2/tagNum_37.html).
            /// @param _val Value assigned to it: string.
            void set37_orderID(const std::string &_val);

            /// @brief Set the targeted order Id [41](https://www.onixs.biz/fix-dictionary/4.2/tagNum_41.html).
            /// @param _val Value assigned to it: string.
            void set41_origClOrdID(const std::string &_val);

            /// @brief Set the status of the order [39](https://www.onixs.biz/fix-dictionary/4.2/tagNum_39.html).
            /// @param _val Value assigned to it: (0, 1, 2, 4, or 8)
            void set39_ordStatus(const std::string &_val);

            /// @brief Set an optional text to explain the reason [58](https://www.onixs.biz/fix-dictionary/4.2/tagNum_58.html).
            /// @param _val Value assigned to it: string.
            void set58_text(const std::string &_val);

            /// @brief Set the type of cancel [434](https://www.onixs.biz/fix-dictionary/4.2/tagNum_434.html).
            /// @param _val Value assigned to it: (1 or 2).
            void set434_cxlRejReason(const std::string &_val);

            static constexpr const char MsgType[] = "9";         ///< Message type value as string.
            static constexpr const char cMsgType = MsgType[0];  ///< Message type value as char.
    };
}