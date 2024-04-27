#include "Common/Core/Utils.hpp"
#include "Common/Message/OrderCancelRequest.hpp"
#include "Common/Message/Reject.hpp"
#include "Common/Message/Tag.hpp"

namespace fix
{
    OrderCancelRequest::OrderCancelRequest()
    {
        header.set35_MsgType(MsgType);
    }

    std::pair<bool, Reject> OrderCancelRequest::Verify(Serializer::AnonMessage &_msg)
    {
        std::pair<bool, Reject> reject = utils::Has<Tag::ClOrdID, Tag::OrigClOrdID, Tag::Side, Tag::Symbol, Tag::TransactTime>(_msg);

        if (reject.first)
            reject = verify_all<Tag::ClOrdID, Tag::OrigClOrdID, Tag::Side, Tag::Symbol, Tag::TransactTime>(_msg);
        return reject;
    }


    void OrderCancelRequest::set11_clOrdID(const std::string &_val)
    {
        m_params.append({ Tag::ClOrdID, _val });
    }

    void OrderCancelRequest::set41_origClOrdID(const std::string &_val)
    {
        m_params.append({ Tag::OrigClOrdID, _val });
    }

    void OrderCancelRequest::set54_side(const std::string &_val)
    {
        m_params.append({ Tag::Side, _val });
    }

    void OrderCancelRequest::set55_symbol(const std::string &_val)
    {
        m_params.append({ Tag::Symbol, _val });
    }

    void OrderCancelRequest::set60_transactTime(const std::string &_val)
    {
        m_params.append({ Tag::TransactTime, _val });
    }
}