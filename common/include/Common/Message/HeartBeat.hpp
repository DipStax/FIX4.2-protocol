#pragma once

#include "Common/Message/Fix.hpp"

namespace fix
{
    /// @brief Fix [HeartBeat](https://www.onixs.biz/fix-dictionary/4.2/msgType_0_0.html) message class.
    class HeartBeat : public Message
    {
        public:
            HeartBeat();
            ~HeartBeat() = default;

            /// @brief Verify if the fix::HeartBeat message receive is correctly formated.
            /// @param _msg Message to check.
            /// @return The first element will always be false.
            static std::pair<bool, Reject> Verify(Serializer::AnonMessage &_msg);

            /// @brief Reference the fix::TestRequest id [112](https://www.onixs.biz/fix-dictionary/4.2/tagNum_112.html).
            /// @param _val Value assigned to it: int.
            void set112_testReqID(const std::string &_val);

            static constexpr const char MsgType[] = "0";         ///< Message type value as string.
            static constexpr const char cMsgType = MsgType[0];  ///< Message type value as char.
    };
}