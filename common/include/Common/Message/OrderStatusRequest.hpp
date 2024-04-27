#pragma once

#include "Common/Message/Fix.hpp"

namespace fix
{
    class OrderStatusRequest : public Message
    {
        public:
            OrderStatusRequest();
            ~OrderStatusRequest() = default;
            // void set37_OrderID(const std::string &_val);
            void set11_ClOrdID(const std::string &_val);
            // void set41_OrigClOrdID(const std::string &_val);
            void set55_Symbol(const std::string &_val);
            void set54_Side(const std::string &_val);

            static constexpr const char MsgType[] = "H";         ///< Message type value as string.
            static constexpr const char cMsgType = MsgType[0];  ///< Message type value as char.
    };
}