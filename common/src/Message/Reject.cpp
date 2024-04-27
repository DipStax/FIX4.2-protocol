#include "Common/Message/Reject.hpp"
#include "Common/Message/Tag.hpp"

namespace fix
{
    Reject::Reject()
    {
        header.set35_MsgType(MsgType);
    }

    void Reject::set45_refSeqNum(const std::string &_val)
    {
        m_params.append({ Tag::RefSeqNum, _val });
    }

    void Reject::set58_text(const std::string &_val)
    {
        m_params.append({ Tag::Text, _val });
    }

    void Reject::set371_refTagId(const std::string &_val)
    {
        m_params.append({ Tag::RefTagId, _val });
    }

    void Reject::set372_refMsgType(const std::string &_val)
    {
        m_params.append({ Tag::RefMsgType, _val });
    }

    void Reject::set373_sessionRejectReason(const std::string &_val)
    {
        m_params.append({ Tag::SessionRejectReason, _val });
    }
}