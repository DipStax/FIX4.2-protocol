#pragma once

#include "Common/Message/Fix.hpp"
#include "Common/Message/Reject.hpp"
#include "Common/Message/Serializer.hpp"

namespace fix
{
    /// @brief Fix [Logon](https://www.onixs.biz/fix-dictionary/4.2/msgType_A_65.html) message class.
    class Logon : public Message
    {
        public:
            Logon();
            ~Logon() = default;

            /// @brief Verify if the fix::Logon message receive is correctly formated.
            /// @param _msg Message to check.
            /// @return If the first element is true then second is set, otherwise it rigly formated.
            static std::pair<bool, Reject> Verify(Serializer::AnonMessage &_msg);

            /// @brief Set the encryption method [98](https://www.onixs.biz/fix-dictionary/4.2/tagNum_98.html).
            /// @param _val Value assigned to it: (0).
            void set98_EncryptMethod(const std::string &_val);

            /// @brief Set the Heart beat rate [108](https://www.onixs.biz/fix-dictionary/4.2/tagNum_108.html).
            /// @param _val Value assigned to it: int.
            void set108_HeartBtInt(const std::string &_val);

            static constexpr const char MsgType[] = "A";        ///< Message type value as string.
            static constexpr const char cMsgType = MsgType[0];  ///< Message type value as char.
    };
}