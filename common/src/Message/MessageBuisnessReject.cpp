#include "Common/Message/BusinessMessageReject.hpp"

#include "Common/Message/Tag.hpp"

namespace fix
{
    BusinessMessageReject::BusinessMessageReject()
    {
        header.set35_MsgType(MsgType);
    }

    void BusinessMessageReject::set45_refSeqNum(const std::string &_val)
    {
        m_params.append({ Tag::RefSeqNum, _val });
    }

    void BusinessMessageReject::set58_text(const std::string &_val)
    {
        m_params.append({ Tag::Text, _val });
    }

    void BusinessMessageReject::set372_refMsgType(const std::string &_val)
    {
        m_params.append({ Tag::RefMsgType, _val });
    }

    void BusinessMessageReject::set379_businessRejectRefId(const std::string &_val)
    {
        m_params.append({ Tag::BusinessRejectRefId, _val });
    }

    void BusinessMessageReject::set380_businessRejectReason(const std::string &_val)
    {
        m_params.append({ Tag::BusinessRejectReason, _val });
    }
}