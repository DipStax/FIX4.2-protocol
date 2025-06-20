#include "Common/Utils.hpp"
#include "Common/Message/HeartBeat.hpp"
#include "Common/Message/Reject.hpp"
#include "Common/Message/Tag.hpp"

namespace fix
{
    HeartBeat::HeartBeat()
    {
        header.set35_MsgType(MsgType);
    }

    std::pair<bool, Reject> HeartBeat::Verify(const Serializer::AnonMessage &_msg)
    {
        std::ignore = _msg;

        return { false, {} };
    }

    void HeartBeat::set112_testReqID(const std::string &_val)
    {
        m_params.append({ Tag::TestReqId, _val });
    }
}