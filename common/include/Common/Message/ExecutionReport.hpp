#pragma once
#include "Common/Message/Fix.hpp"

namespace fix
{
    /// @brief Fix [Execution Report](https://www.onixs.biz/fix-dictionary/4.2/msgType_8_8.html) message class.
    class ExecutionReport : public Message
    {
        public:
            ExecutionReport();
            ~ExecutionReport() = default;

            /// @brief Calculated average price of all fills on this order.
            /// @param _val Value assigned to it: double.
            void set6_avgPx(const std::string &_val);

            /// @brief Quantity sold/bought after the execution of the order [14](https://www.onixs.biz/fix-dictionary/4.2/tagNum_14.html).
            /// @param _val Value assigned to it: double.
            void set14_cumQty(const std::string &_val);

            /// @brief Set the id of the order's execution [14](https://www.onixs.biz/fix-dictionary/4.2/tagNum_14.html).
            /// @param _val Value assigned to it: int.
            [[deprecated("Used directly void set17_execID() to automaticly increment the ExecId")]]
            void set17_execID(const std::string &_val);
            /// @brief Set the id of the order's execution automaticly from ExecId [14](https://www.onixs.biz/fix-dictionary/4.2/tagNum_14.html).
            void set17_execID();

            /// @brief Set the report type [20](https://www.onixs.biz/fix-dictionary/4.2/tagNum_20.html).
            /// @param _val Value assigned to it: (0, 1, 2, 3)
            void set20_execTransType(const std::string &_val);

            /// @brief Set the reference order Id of the report [37](https://www.onixs.biz/fix-dictionary/4.2/tagNum_37.html).
            /// @param _val Value assigned to it: string.
            void set37_orderID(const std::string &_val);

            /// @brief Set the quantity order [38](https://www.onixs.biz/fix-dictionary/4.2/tagNum_38.html).
            /// @param _val Value assigned to it: double.
            void set38_orderQty(const std::string &_val);

            /// @brief Set the status of the order [39](https://www.onixs.biz/fix-dictionary/4.2/tagNum_39.html).
            /// @param _val Value assigned to it: (0, 1, 2, 4, or 8)
            void set39_ordStatus(const std::string &_val);

            /// @brief Set the type of the order [40](https://www.onixs.biz/fix-dictionary/4.2/tagNum_40.html).
            /// @param _val Value assigned to it: (2).
            void set40_ordType(const std::string &_val);

            /// @brief Set the Original order id [41](https://www.onixs.biz/fix-dictionary/4.2/tagNum_41.html).
            /// @param _val Value assigned to it: string.
            void set41_origClOrdID(const std::string &_val);

            /// @brief Set the price of the order [44](https://www.onixs.biz/fix-dictionary/4.2/tagNum_44.html).
            /// @param _val Value assigned to it: double.
            void set44_price(const std::string &_val);

            /// @brief Set the strategy of buying/selling [54](https://www.onixs.biz/fix-dictionary/4.2/tagNum_54.html).
            /// @param _val Value assigned ot it: (3 or 4).
            void set54_side(const std::string &_val);

            /// @brief Set the symbol of the target market to trad on [55](https://www.onixs.biz/fix-dictionary/4.2/tagNum_55.html).
            /// @param _val Value assigned to it: string.
            void set55_symbol(const std::string &_val);

            /// @brief Set an optional text to explain the reason [58](https://www.onixs.biz/fix-dictionary/4.2/tagNum_58.html).
            /// @param _val Value assigned to it: string.
            void set58_text(const std::string &_val);

            /// @brief Set the type of execution on the order [150](https://www.onixs.biz/fix-dictionary/4.2/tagNum_150.html).
            /// @param _val Value assigned to it: (0, 1, 2, 4, or 8)
            void set150_execType(const std::string &_val);

            /// @brief Remaining quantity after executing the order [151](https://www.onixs.biz/fix-dictionary/4.2/tagNum_151.html).
            /// @param _val Value assigned to it: double.
            void set151_leavesQty(const std::string &_val);


            static inline size_t ExecId = 1;                    ///< Auto incremented [execution id](https://www.onixs.biz/fix-dictionary/4.2/tagNum_14.html).

            static constexpr const char MsgType[] = "8";        ///< Message type value as string.
            static constexpr const char cMsgType = MsgType[0];  ///< Message type value as char.
    };
}