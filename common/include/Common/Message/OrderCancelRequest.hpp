#pragma once
#include "Common/Message/Fix.hpp"
#include "Common/Message/Reject.hpp"

namespace fix
{
    /// @brief Fix [Order Cancel Request](https://www.onixs.biz/fix-dictionary/4.2/msgType_9_9.html) class.
    class OrderCancelRequest : public Message
    {
        public:
            OrderCancelRequest();
            ~OrderCancelRequest() = default;

            /// @brief Verify if the logon message receive is correctly formated.
            /// @param _msg Message to check.
            /// @return If the first element is true then second is set, otherwise it rigly formated.
            static std::pair<bool, Reject> Verify(Serializer::AnonMessage &_msg);

            /// @brief Set the order Id [11](https://www.onixs.biz/fix-dictionary/4.2/tagNum_11.html).
            /// @param _val Value assigned to it: string.
            void set11_clOrdID(const std::string &_val);

            /// @brief Set the targeted order Id [41](https://www.onixs.biz/fix-dictionary/4.2/tagNum_41.html).
            /// @param _val Value assigned to it: string.
            void set41_origClOrdID(const std::string &_val);

            /// @brief Set the symbol of the target market to trad on [55](https://www.onixs.biz/fix-dictionary/4.2/tagNum_21.html).
            /// @param _val Value assigned to it: string.
            void set55_symbol(const std::string &_val);

            /// @brief Set the strategy of buying/selling [54](https://www.onixs.biz/fix-dictionary/4.2/tagNum_54.html).
            /// @param _val Value assigned ot it: (3 or 4).
            void set54_side(const std::string &_val);

            /// @brief Set the time of the transaction [60](https://www.onixs.biz/fix-dictionary/4.2/tagNum_60.html).
            /// @param _val Value assigned to it: UTC timestamp.
            void set60_transactTime(const std::string &_val);

            static constexpr const char MsgType[] = "F";         ///< Message type value as string.
            static constexpr const char cMsgType = MsgType[0];  ///< Message type value as char.
    };
}