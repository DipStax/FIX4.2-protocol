#pragma once

#include "Common/Message/Fix.hpp"

namespace fix
{
    class MarketDataRequest : public Message
    {
        public:
            static std::pair<bool, Reject> Verify(Serializer::AnonMessage &_msg);

            MarketDataRequest();
            ~MarketDataRequest() = default;

            /// @brief Symbol requested [55](https://www.onixs.biz/fix-dictionary/4.2/tagNum_55.html).
            /// @param _val Value assigned to it: string.
            void set55_symbol(const std::string &_val);

            /// @brief Number of symbol requested [146](https://www.onixs.biz/fix-dictionary/4.2/tagNum_146.html).
            /// @param _val Value assigned to it: int.
            void set146_noRelatedSym(const std::string &_val);

            /// @brief Request Id [262](https://www.onixs.biz/fix-dictionary/4.2/tagNum_262.html).
            /// @param _val Value assigned to it: string.
            void set262_mDReqID(const std::string &_val);

            /// @brief Set the type of subscription [263](https://www.onixs.biz/fix-dictionary/4.2/tagNum_263.html).
            /// @param _val Value assigned to it: (0, 1 or 2).
            void set263_subscriptionRequestType(const std::string &_val);

            /// @brief Set the market depth [264](https://www.onixs.biz/fix-dictionary/4.2/tagNum_264.html).
            /// @param _val Value assigned to it: (0, 1 or N>1).
            void set264_marketDepth(const std::string &_val);

            /// @brief Number of data type requested [267](https://www.onixs.biz/fix-dictionary/4.2/tagNum_267.html).
            /// @param _val Value assigned to it: int.
            void set267_noMDEntryTypes(const std::string &_val);

            /// @brief Type of data requested on symnol [269](https://www.onixs.biz/fix-dictionary/4.2/tagNum_269.html).
            /// @param _val Value assigned to it: (0 or 1)
            void set269_mDEntryType(const std::string &_val);

            static constexpr const char MsgType[] = "V";        ///< Message type value as string.
            static constexpr const char cMsgType = MsgType[0];  ///< Message type value as char.
    };
}