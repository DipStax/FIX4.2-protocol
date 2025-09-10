#include "Shared/Utils/Utils.hpp"
#include "Shared/Message/NewOrderSingle.hpp"
#include "Shared/Message/Tag.hpp"

namespace fix
{
    NewOrderSingle::NewOrderSingle()
    {
        header.set35_MsgType(MsgType);
    }

    std::pair<bool, Reject> NewOrderSingle::Verify(Serializer::AnonMessage &_msg)
    {
        // need to verify transaction time, symbol
        std::pair<bool, Reject> reject = Has<Tag::ClOrdID, Tag::HandlInst,
            Tag::OrderQty, Tag::OrdType, Tag::Price, Tag::Side, Tag::Symbol>(_msg);

        reject.second.set45_refSeqNum(NewOrderSingle::MsgType);
        if (reject.first)
            return reject;
        reject = verify_all<Tag::ClOrdID, Tag::HandlInst, Tag::OrderQty, Tag::OrdType,
            Tag::Price, Tag::Side, Tag::Symbol>(_msg);
        return reject;
    }

    void NewOrderSingle::set11_clOrdID(const std::string &_val)
    {
        m_params.append({ Tag::ClOrdID, _val });
    }

    void NewOrderSingle::set21_handlInst(const std::string &_val)
    {
        m_params.append({ Tag::HandlInst, _val });
    }

    void NewOrderSingle::set38_orderQty(const std::string &_val)
    {
        m_params.append({ Tag::OrderQty, _val });
    }

    void NewOrderSingle::set40_ordType(const std::string &_val)
    {
        m_params.append({ Tag::OrdType, _val });
    }

    void NewOrderSingle::set44_price(const std::string &_val)
    {
        m_params.append({ Tag::Price, _val });
    }

    void NewOrderSingle::set54_side(const std::string &_val)
    {
        m_params.append({ Tag::Side, _val });
    }

    void NewOrderSingle::set55_symbol(const std::string &_val)
    {
        m_params.append({ Tag::Symbol, _val });
    }

    void NewOrderSingle::set60_transactTime(const std::string &_val)
    {
        m_params.append({ Tag::TransactTime, _val });
    }
}