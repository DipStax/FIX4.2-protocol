#include "Common/Message/OrderCancelReject.hpp"
#include "Common/Message/Tag.hpp"

namespace fix
{
    OrderCancelReject::OrderCancelReject()
    {
        header.set35_MsgType(MsgType);
    }

    void OrderCancelReject::set11_clOrdID(const std::string &_val)
    {
        m_params.append({ fix::Tag::ClOrdID, _val });
    }

    void OrderCancelReject::set37_orderID(const std::string &_val)
    {
        m_params.append({ fix::Tag::OrderID, _val });
    }

    void OrderCancelReject::set39_ordStatus(const std::string &_val)
    {
        m_params.append({ fix::Tag::OrdStatus, _val });
    }

    void OrderCancelReject::set41_origClOrdID(const std::string &_val)
    {
        m_params.append({ fix::Tag::OrigClOrdID, _val });
    }

    void OrderCancelReject::set58_text(const std::string &_val)
    {
        m_params.append({ Tag::Text, _val });
    }

    void OrderCancelReject::set434_cxlRejReason(const std::string &_val)
    {
        m_params.append({ "434", _val });
    }
}