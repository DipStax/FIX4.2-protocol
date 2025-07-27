#include "Shared/Utils/Utils.hpp"
#include "Shared/Message/HeartBeat.hpp"
#include "Shared/Message/Reject.hpp"
#include "Shared/Message/Tag.hpp"

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