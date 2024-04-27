#include "Common/Core/Utils.hpp"
#include "Common/Message/MarketDataRequest.hpp"
#include "Common/Message/Tag.hpp"

namespace fix
{
    std::pair<bool, Reject> MarketDataRequest::Verify(Serializer::AnonMessage &_msg)
    {
        std::pair<bool, Reject> reject = utils::Has<Tag::Symbol, Tag::NoRelatedSym,Tag::MDReqID,
            Tag::SubscriptionRequestType,Tag::MarketDepth, Tag::NoMDEntryTypes, Tag::MDEntryType>(_msg);

        if (reject.first)
            return reject;
        reject = verify_all<Tag::NoRelatedSym, Tag::SubscriptionRequestType, Tag::MarketDepth, Tag::NoMDEntryTypes>(_msg);
        if (reject.first)
            return reject;
        reject = verify<Tag::Symbol>(_msg.at(Tag::Symbol), utils::to<size_t>(_msg.at(Tag::NoRelatedSym)));
        if (reject.first)
            return reject;
        reject = verify<Tag::Symbol>(_msg.at(Tag::MDEntryType), utils::to<size_t>(_msg.at(Tag::NoMDEntryTypes)));
        return reject;
    }

    MarketDataRequest::MarketDataRequest()
    {
        header.set35_MsgType(MsgType);
    }

    void MarketDataRequest::set55_symbol(const std::string &_val)
    {
        m_params.append({ Tag::Symbol, _val });
    }

    void MarketDataRequest::set146_noRelatedSym(const std::string &_val)
    {
        m_params.append({ Tag::NoRelatedSym, _val });
    }

    void MarketDataRequest::set262_mDReqID(const std::string &_val)
    {
        m_params.append({ Tag::MDReqID, _val });
    }

    void MarketDataRequest::set263_subscriptionRequestType(const std::string &_val)
    {
        m_params.append({ Tag::SubscriptionRequestType, _val });
    }

    void MarketDataRequest::set264_marketDepth(const std::string &_val)
    {
        m_params.append({ Tag::MarketDepth, _val });
    }

    void MarketDataRequest::set267_noMDEntryTypes(const std::string &_val)
    {
        m_params.append({ Tag::NoMDEntryTypes, _val });
    }

    void MarketDataRequest::set269_mDEntryType(const std::string &_val)
    {
        m_params.append({ Tag::MDEntryType, _val });
    }
}