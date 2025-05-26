#pragma once

#include "Common/Message/Fix.hpp"

namespace fix
{
    /// @brief Fix [TestRequest](https://www.onixs.biz/fix-dictionary/4.2/msgType_1_1.html) message class.
    class TestRequest : public Message
    {
        public:
            TestRequest();
            ~TestRequest() = default;

            /// @brief Verify if the fix::TestRequest message receive is correctly formated.
            /// @param _msg Message to check.
            /// @return The first element will always be false.
            static std::pair<bool, Reject> Verify(Serializer::AnonMessage &_msg);

            /// @brief Reference the fix::TestRequest id [112](https://www.onixs.biz/fix-dictionary/4.2/tagNum_112.html).
            /// @param _val Value assigned to it: int.
            void set112_testReqID(const std::string &_val);

            static constexpr const char MsgType[] = "1";         ///< Message type value as string.
            static constexpr const char cMsgType = MsgType[0];  ///< Message type value as char.
    };
}