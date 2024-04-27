#pragma once

#include "Common/Message/Fix.hpp"

namespace fix
{
    /**
     * @brief Market Data messages are used as the response to a Market Data Request <V> message.
    */
    class MarketDataSnapshotFullRefresh: public Message
    {
        public:
            MarketDataSnapshotFullRefresh();
            ~MarketDataSnapshotFullRefresh() = default;

            /// @brief Symbol requested [55](https://www.onixs.biz/fix-dictionary/4.2/tagNum_55.html).
            /// @param _val Value assigned to it: string.
            void set55_symbol(const std::string &_val);

            /// @brief Quantity [110](https://www.onixs.biz/fix-dictionary/4.2/tagNum_110.html).
            /// @param _val Value assigned to it: (double)
            void set110_minQty(const std::string &_val);

            /// @brief Request Id [262](https://www.onixs.biz/fix-dictionary/4.2/tagNum_262.html).
            /// @param _val Value assigned to it: string.
            void set262_mDReqID(const std::string &_val);

            /// @brief Number of data type requested [267](https://www.onixs.biz/fix-dictionary/4.2/tagNum_267.html).
            /// @param _val Value assigned to it: int.
            void set267_noMDEntryTypes(const std::string &_val);

            /// @brief Type of data requested on symnol [269](https://www.onixs.biz/fix-dictionary/4.2/tagNum_269.html).
            /// @param _val Value assigned to it: (0 or 1)
            void set269_mDEntryType(const std::string &_val);

            /// @brief Price of the entry [270](https://www.onixs.biz/fix-dictionary/4.2/tagNum_270.html).
            /// @param _val Value assigned to it: (double)
            void set270_mDEntryPx(const std::string &_val);

            MarketDataSnapshotFullRefresh &operator+=(MarketDataSnapshotFullRefresh _ref);

            static constexpr const char MsgType[] = "V";        ///< Message type value as string.
            static constexpr const char cMsgType = MsgType[0];  ///< Message type value as char.
    };
}