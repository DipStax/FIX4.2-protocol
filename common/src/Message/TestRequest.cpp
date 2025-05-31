#include "Common/Utils.hpp"
#include "Common/Message/TestRequest.hpp"
#include "Common/Message/Reject.hpp"
#include "Common/Message/Tag.hpp"

namespace fix
{
    TestRequest::TestRequest()
    {
        header.set35_MsgType(MsgType);
    }

    std::pair<bool, Reject> TestRequest::Verify(Serializer::AnonMessage &_msg)
    {
        std::pair<bool, Reject> reject = { true, {} };

        reject.second.set371_refTagId(TestRequest::MsgType);
        if (!_msg.contains(Tag::TestReqId)) {
            reject.second.set371_refTagId(Tag::TestReqId);
            reject.second.set373_sessionRejectReason(Reject::ReqTagMissing);
            reject.second.set58_text("Missing TestRequest");
        } else {
            reject.first = false;
        }
        return reject;
    }

    void TestRequest::set112_testReqID(const std::string &_val)
    {
        m_params.append({ Tag::TestReqId, _val });
    }
}