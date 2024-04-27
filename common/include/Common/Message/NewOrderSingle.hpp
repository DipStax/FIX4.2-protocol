#pragma once

#include "Common/Message/Fix.hpp"

namespace fix
{
    /// @brief Fix [New Order Single](https://www.onixs.biz/fix-dictionary/4.2/msgType_D_68.html) message class.
    class NewOrderSingle : public Message
    {
        public:
            NewOrderSingle();
            ~NewOrderSingle() = default;

            /// @brief Verify if the fix::NewOrderSingle message receive is correctly formated.
            /// @param _msg Message to check.
            /// @return If the first element is true then second is set, otherwise it rigly formated.
            static std::pair<bool, Reject> Verify(Serializer::AnonMessage &_msg);

            /// @brief Set the order Id [11](https://www.onixs.biz/fix-dictionary/4.2/tagNum_11.html).
            /// @param _val Value assigned to it: string.
            void set11_clOrdID(const std::string &_val);

            /// @brief Set the trading strategy of the order [21](https://www.onixs.biz/fix-dictionary/4.2/tagNum_21.html).
            /// @param _val Value assigned to it: (3).
            void set21_handlInst(const std::string &_val);

            /// @brief Set the quantity order [38](https://www.onixs.biz/fix-dictionary/4.2/tagNum_38.html).
            /// @param _val Value assigned to it: double.
            void set38_orderQty(const std::string &_val);

            /// @brief Set the type of the order [40](https://www.onixs.biz/fix-dictionary/4.2/tagNum_40.html).
            /// @param _val Value assigned to it: (2).
            void set40_ordType(const std::string &_val);

            /// @brief Set the price of the order [44](https://www.onixs.biz/fix-dictionary/4.2/tagNum_44.html).
            /// @param _val Value assigned to it: double.
            void set44_price(const std::string &_val);

            /// @brief Set the strategy of buying/selling [54](https://www.onixs.biz/fix-dictionary/4.2/tagNum_54.html).
            /// @param _val Value assigned ot it: (3 or 4). (3 = Buy, 4 = Sell)
            void set54_side(const std::string &_val);

            /// @brief Set the symbol of the target market to trad on [55](https://www.onixs.biz/fix-dictionary/4.2/tagNum_21.html).
            /// @param _val Value assigned to it: string.
            void set55_symbol(const std::string &_val);

            /// @brief Set the time of the transaction [60](https://www.onixs.biz/fix-dictionary/4.2/tagNum_60.html).
            /// @param _val Value assigned to it: UTC timestamp.
            void set60_transactTime(const std::string &_val);

            static constexpr const char MsgType[] = "D";         ///< Message type value as string.
            static constexpr const char cMsgType = MsgType[0];  ///< Message type value as char.
    };
}