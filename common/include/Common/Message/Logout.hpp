#pragma once

#include "Common/Message/Fix.hpp"

namespace fix
{
    /// @brief Fix [Logout](https://www.onixs.biz/fix-dictionary/4.2/msgType_5_5.html) message class.
    class Logout : public Message
    {
        public:
            Logout();
            ~Logout() = default;

            /// @brief Verify if the fix::Logout message receive is correctly formated.
            /// @param _msg Message to check.
            /// @return The first element will always be false.
            static std::pair<bool, Reject> Verify(Serializer::AnonMessage &_msg);

            static constexpr const char MsgType[] = "5";        ///< Message type value as string.
            static constexpr const char cMsgType = MsgType[0];  ///< Message type value as char.
    };
}