#pragma once

#include "Message/Fix.hpp"

namespace fix
{
    class Logon : public Message
    {
        void set49_EncryptMedthod(const std::string &_val); // EncryptMedthod
        void set108_HeartBtInt(const std::string &_val); // HeartBtInt
    };
}