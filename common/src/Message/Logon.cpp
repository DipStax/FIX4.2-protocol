#include "Common/Core/Utils.hpp"
#include "Common/Message/Logon.hpp"
#include "Common/Message/Tag.hpp"

namespace fix
{
    Logon::Logon()
    {
        header.set35_MsgType(MsgType);
    }

    std::pair<bool, Reject> Logon::Verify(Serializer::AnonMessage &_msg)
    {
        std::pair<bool, Reject> reject = utils::Has<Tag::EncryptMethod, Tag::HearBtInt>(_msg);

        if (!reject.first)
            reject = verify_all<Tag::EncryptMethod, Tag::HearBtInt>(_msg);
        reject.second.set372_refMsgType(Logon::MsgType);
        return reject;
    }

    void Logon::set98_EncryptMethod(const std::string &_val)
    {
        m_params.append({ Tag::EncryptMethod, _val });
    }

    void Logon::set108_HeartBtInt(const std::string &_val)
    {
        m_params.append({ Tag::HearBtInt, _val });
    }
}