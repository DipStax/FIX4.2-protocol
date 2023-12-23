#pragma once

#include "Common/Message/Fix.hpp"

namespace fix
{
    class HeartBeat: public Message
    {
    public:
        HeartBeat();
        ~HeartBeat();
        /**
         * NOT REQUIRED
         * @param _val
         * Identifier included in Test Request <1> message to be returned in Heartbeat <0> message.
         */
        void set112_TestReqID(const std::string &_val);
    protected:
        static constexpr const char *m_msgType = "0";
    };
}