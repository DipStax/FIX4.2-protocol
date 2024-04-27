#include "Common/Core/Utils.hpp"
#include "Common/Message/HeartBeat.hpp"
#include "Common/Message/Reject.hpp"
#include "Common/Message/Tag.hpp"

namespace fix
{
    HeartBeat::HeartBeat()
    {
        header.set35_MsgType(MsgType);
    }

    std::pair<bool, Reject> HeartBeat::Verify(Serializer::AnonMessage &_msg)
    {
        std::pair<bool, Reject> reject = { true, {} };

        reject.second.set371_refTagId(HeartBeat::MsgType);
        if (_msg.contains(Tag::TestReqId) && !utils::is_numeric(_msg.at(Tag::TestReqId))) {
            reject.second.set371_refTagId(Tag::TestReqId);
            reject.second.set373_sessionRejectReason(Reject::IncorrectFormat);
            reject.second.set58_text("Not supported test Id");
        } else {
            reject.first = false;
        }
        return reject;
    }

    void HeartBeat::set112_testReqID(const std::string &_val)
    {
        m_params.append({ Tag::TestReqId, _val });
    }
}