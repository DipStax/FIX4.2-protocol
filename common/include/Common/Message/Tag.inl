#include <string>

#include "Common/Message/Message.hpp"
#include "Common/Message/Tag.hpp"
#include "Common/Core/Utils.hpp"

namespace fix
{
    template<const char *T, const char *...Ts>
    std::pair<bool, Reject> verify_all(Serializer::AnonMessage &_msg)
    {
        std::pair<bool, fix::Reject> reject = verify<T>(_msg.at(T));

        if (reject.first)
            return reject;
        if constexpr (sizeof...(Ts) == 0)
            return reject;
        else
            return verify_all<Ts...>(_msg);
    }

    template<const char *T>
    std::pair<bool, Reject> verify(const std::string &_value)
    {
        // the definition of the specialization are made after the static_assert(false), so it need to be calculated to have the instance of the specialization
        static_assert(!std::is_same_v<T, T>, "No specialization for the verification of this tag");
        return { true,  {} };
    }

    template<const char *T>
    std::pair<bool, Reject> verify(const std::string &, const std::string &)
    {
        // the definition of the specialization are made after the static_assert(false), so it need to be calculated to have the instance of the specialization
        static_assert(!std::is_same_v<T, T>, "No specialization for the verification of this tag");
        return { true,  {} };
    }

    template<const char *T>
    std::pair<bool, Reject> verify(const std::string &, size_t)
    {
        // the definition of the specialization are made after the static_assert(false), so it need to be calculated to have the instance of the specialization
        static_assert(!std::is_same_v<T, T>, "No specialization for the verification of this tag");
        return { true,  {} };
    }

    template<>
    std::pair<bool, Reject> verify<Tag::BeginString>(const std::string &_value)
    {
        std::pair<bool, fix::Reject> reject = { false, {} };

        if (_value != "FIX.4.2") {
            reject.first = true;
            reject.second.set371_refTagId(Tag::BeginString);
            reject.second.set373_sessionRejectReason(Reject::ValueOORange);
            reject.second.set58_text("Supported version is FIX.4.2");
        }
        return reject;
    }

    template<>
    std::pair<bool, Reject> verify<Tag::BodyLength>(const std::string &_value, size_t _len)
    {
        std::pair<bool, fix::Reject> reject = { false, {} };

        reject.second.set371_refTagId(Tag::BodyLength);
        if (!utils::is_numeric(_value)) {
            reject.first = true;
            reject.second.set373_sessionRejectReason(Reject::IncorrectFormat);
            reject.second.set58_text("Body length should be numeric");
        } else if (utils::to<size_t>(_value) != _len) {
            reject.first = true;
            reject.second.set373_sessionRejectReason(Reject::ValueOORange);
            reject.second.set58_text("Body length is not correct");
        }
        return reject;
    }

    template<>
    std::pair<bool, Reject> verify<Tag::CheckSum>(const std::string &_value, const std::string &_checksum)
    {
        std::pair<bool, fix::Reject> reject = { false, {} };

        if (_checksum != _value) {
            reject.first = true;
            reject.second.set371_refTagId(Tag::CheckSum);
            reject.second.set373_sessionRejectReason(Reject::ValueOORange);
            reject.second.set58_text("Checksum is not correct");
        }
        return reject;
    }

    template<>
    std::pair<bool, Reject> verify<Tag::MsgType>(const std::string &_value)
    {
        constexpr const char type[] = {
            Logon::cMsgType,
            HeartBeat::cMsgType,
            NewOrderSingle::cMsgType,
            OrderCancelRequest::cMsgType,
            OrderCancelReplaceRequest::cMsgType,
            MarketDataRequest::cMsgType,
            Logout::cMsgType
        };
        constexpr const size_t size_type = 7;
        std::pair<bool, fix::Reject> reject = { false, {} };

        for (size_t i = 0; i < size_type; i++)
            if (type[i] == _value[0])
                return reject;
        reject.first = true;
        reject.second.set371_refTagId(Tag::MsgType);
        reject.second.set373_sessionRejectReason(Reject::NotSupporType);
        reject.second.set58_text("Not supported message type");
        return reject;
    }

    template<>
    std::pair<bool, Reject> verify<Tag::ClOrdID>(const std::string &_value)
    {
        std::pair<bool, fix::Reject> reject = { false, {} };

        if (!utils::is_numeric(_value)) {
            reject.first = true;
            reject.second.set371_refTagId(Tag::ClOrdID);
            reject.second.set373_sessionRejectReason(Reject::IncorrectFormat);
            reject.second.set58_text("Not supported order Id");
        }
        return reject;
    }

    template<>
    std::pair<bool, Reject> verify<Tag::HandlInst>(const std::string &_value)
    {
        std::pair<bool, fix::Reject> reject = { false, {} };

        if (_value != "3") {
            reject.first = true;
            reject.second.set371_refTagId(Tag::HandlInst);
            reject.second.set373_sessionRejectReason(Reject::ValueOORange);
            reject.second.set58_text("Not supported order Id");
        }
        return reject;
    }

    template<>
    std::pair<bool, Reject> verify<Tag::MsqSeqNum>(const std::string &_value, size_t _seqnum)
    {
        std::pair<bool, fix::Reject> reject = { false, {} };

        reject.second.set371_refTagId(Tag::MsqSeqNum);
        if (!utils::is_numeric(_value)) {
            reject.first = true;
            reject.second.set373_sessionRejectReason(Reject::IncorrectFormat);
            reject.second.set58_text("Sequence number should be numeric");
        } else if (utils::to<size_t>(_value) != _seqnum) {
            reject.first = true;
            reject.second.set373_sessionRejectReason(Reject::ValueOORange);
            reject.second.set58_text("Sequence number is not correct");
        }
        return reject;
    }

    template<>
    std::pair<bool, Reject> verify<Tag::OrderQty>(const std::string &_value)
    {
        std::pair<bool, fix::Reject> reject = { false, {} };

        if (!utils::is_double(_value)) {
            reject.first = true;
            reject.second.set371_refTagId(Tag::OrderQty);
            reject.second.set373_sessionRejectReason(Reject::IncorrectFormat);
            reject.second.set58_text("Quantity should be a double");
        }
        return reject;
    }

    template<>
    std::pair<bool, Reject> verify<Tag::OrdType>(const std::string &_value)
    {
        std::pair<bool, fix::Reject> reject = { false, {} };

        if (_value != "2") {
            reject.first = true;
            reject.second.set371_refTagId(Tag::OrdType);
            reject.second.set373_sessionRejectReason(Reject::ValueOORange);
            reject.second.set58_text("Order type not supported");
        }
        return reject;
    }

    template<>
    std::pair<bool, Reject> verify<Tag::Price>(const std::string &_value)
    {
        std::pair<bool, fix::Reject> reject = { false, {} };

        if (!utils::is_double(_value)) {
            reject.first = true;
            reject.second.set371_refTagId(Tag::Price);
            reject.second.set373_sessionRejectReason(Reject::IncorrectFormat);
            reject.second.set58_text("Price should be a double");
        }
        return reject;
    }

    template<>
    std::pair<bool, Reject> verify<Tag::SenderCompId>(const std::string &_value, const UserId &_sender)
    {
        std::pair<bool, fix::Reject> reject = { false, {} };

        if (_value != _sender) {
            reject.first = true;
            reject.second.set371_refTagId(Tag::SenderCompId);
            reject.second.set373_sessionRejectReason(Reject::ValueOORange);
            reject.second.set58_text("Wrong sender Id");
        }
        return reject;
    }

    template<>
    std::pair<bool, Reject> verify<Tag::SendingTime>(const std::string &_value)
    {
        std::ignore = _value;

        return { false, {} };
    }

    template<>
    std::pair<bool, Reject> verify<Tag::Side>(const std::string &_value)
    {
        std::pair<bool, fix::Reject> reject = { false, {} };

        if (_value != "3" && _value != "4") {
            reject.first = true;
            reject.second.set371_refTagId(Tag::Side);
            reject.second.set373_sessionRejectReason(Reject::IncorrectFormat);
            reject.second.set58_text("Value not supported (only 3 or 4 are allowed)");
        }
        return reject;
    }

    template<>
    std::pair<bool, Reject> verify<Tag::TargetCompId>(const std::string &_value, const UserId &_target)
    {
        std::pair<bool, fix::Reject> reject = { false, {} };

        if (_value != _target) {
            reject.first = true;
            reject.second.set371_refTagId(Tag::SenderCompId);
            reject.second.set373_sessionRejectReason(Reject::ValueOORange);
            reject.second.set58_text("Wrong target Id");
        }
        return reject;
    }


    template<>
    std::pair<bool, Reject> verify<Tag::TransactTime>(const std::string &_value)
    {
        std::ignore = _value;

        return { false, {} };
    }

    template<>
    std::pair<bool, Reject> verify<Tag::Symbol>(const std::string &_value)
    {
        const std::vector<std::string> sym{ MARKET_NAME };
        std::pair<bool, fix::Reject> reject = { false, {} };
        auto it = std::find_if(sym.begin(), sym.end(), [_value] (const std::string &_sym) {
            return _value == _sym;
        });

        if (it == sym.end()) {
            reject.first = true;
            reject.second.set371_refTagId(Tag::Symbol);
            reject.second.set373_sessionRejectReason(Reject::ValueOORange);
            reject.second.set58_text("Not supported symbol in the group");
        }
        return reject;
    }

    template<>
    std::pair<bool, Reject> verify<Tag::Symbol>(const std::string &_value, size_t _n)
    {
        const std::vector<std::string> sym{ MARKET_NAME };
        std::pair<bool, fix::Reject> reject = { false, {} };
        std::vector<std::string> symbols = utils::split<','>(_value);

        if (symbols.size() != _n) {
            reject.first = true;
            reject.second.set371_refTagId(Tag::Symbol);
            reject.second.set373_sessionRejectReason(Reject::IncorrectFormat);
            reject.second.set58_text("The number of symbole doesn't match the number of group");
        } else {
            for (const auto &_symbol : symbols) {
                auto it = std::find_if(sym.begin(), sym.end(), [_symbol] (const std::string &_sym) {
                    return _symbol == _sym;
                });

                if (it == sym.end()) {
                    reject.first = true;
                    reject.second.set371_refTagId(Tag::Symbol);
                    reject.second.set373_sessionRejectReason(Reject::ValueOORange);
                    reject.second.set58_text("Not supported symbol");
                    return reject;
                }
            }
        }
        return reject;
    }

    template<>
    std::pair<bool, Reject> verify<Tag::OrigClOrdID>(const std::string &_value)
    {
        std::pair<bool, fix::Reject> reject = { false, {} };

        if (!utils::is_numeric(_value)) {
            reject.first = true;
            reject.second.set371_refTagId(Tag::OrigClOrdID);
            reject.second.set373_sessionRejectReason(Reject::IncorrectFormat);
            reject.second.set58_text("The origin Id should be numerical");
        }
        return reject;
    }

    template<>
    std::pair<bool, Reject> verify<Tag::EncryptMethod>(const std::string &_value)
    {
        std::pair<bool, fix::Reject> reject = { false, {} };

        if (_value != "0") {
            reject.first = true;
            reject.second.set371_refTagId(Tag::EncryptMethod);
            reject.second.set373_sessionRejectReason(Reject::DecryptionIssue);
            reject.second.set58_text("Not supported encryption method");
        }
        return reject;
    }

    template<>
    std::pair<bool, Reject> verify<Tag::HearBtInt>(const std::string &_value)
    {
        std::pair<bool, fix::Reject> reject = { false, {} };

        if (!utils::is_numeric(_value)) {
            reject.first = true;
            reject.second.set371_refTagId(Tag::HearBtInt);
            reject.second.set373_sessionRejectReason(Reject::IncorrectFormat);
            reject.second.set58_text("Heart beat should be a numerical value");
        }
        return reject;
    }

    template<>
    std::pair<bool, Reject> verify<Tag::NoRelatedSym>(const std::string &_value)
    {
        std::pair<bool, fix::Reject> reject = { false, {} };

        if (!utils::is_numeric(_value)) {
            reject.first = true;
            reject.second.set371_refTagId(Tag::NoRelatedSym);
            reject.second.set373_sessionRejectReason(Reject::IncorrectFormat);
            reject.second.set58_text("Number of related symbol should be a numerical value");
        }
        return reject;
    }

    template<>
    std::pair<bool, Reject> verify<Tag::SubscriptionRequestType>(const std::string &_value)
    {
        std::pair<bool, fix::Reject> reject = { false, {} };

        reject.second.set371_refTagId(Tag::SubscriptionRequestType);
        if (!utils::is_numeric(_value)) {
            reject.first = true;
            reject.second.set373_sessionRejectReason(Reject::IncorrectFormat);
            reject.second.set58_text("Subscription type should be a numerical value");
        } else if (_value != "0" && _value != "1" && _value != "3") {
            reject.first = true;
            reject.second.set373_sessionRejectReason(Reject::ValueOORange);
            reject.second.set58_text("Not supported subscrition type");
        }
        return reject;
    }

    template<>
    std::pair<bool, Reject> verify<Tag::MarketDepth>(const std::string &_value)
    {
        std::pair<bool, fix::Reject> reject = { false, {} };

        reject.second.set371_refTagId(Tag::MarketDepth);
        if (!utils::is_numeric(_value)) {
            reject.first = true;
            reject.second.set373_sessionRejectReason(Reject::IncorrectFormat);
            reject.second.set58_text("Market depth shoudl be a numeric value");
        }
        return reject;
    }

    template<>
    std::pair<bool, Reject> verify<Tag::NoMDEntryTypes>(const std::string &_value)
    {
        std::pair<bool, fix::Reject> reject = { false, {} };

        reject.second.set371_refTagId(Tag::NoMDEntryTypes);
        if (!utils::is_numeric(_value)) {
            reject.first = true;
            reject.second.set373_sessionRejectReason(Reject::IncorrectFormat);
            reject.second.set58_text("Market depth shoudl be a numeric value");
        }
        return reject;
    }

    template<>
    std::pair<bool, Reject> verify<Tag::MDEntryType>(const std::string &_value, size_t _n)
    {
        std::pair<bool, fix::Reject> reject = { false, {} };
        std::vector<std::string> entrys = utils::split<','>(_value);

        if (entrys.size() != _n) {
            reject.first = true;
            reject.second.set371_refTagId(Tag::MDEntryType);
            reject.second.set373_sessionRejectReason(Reject::IncorrectFormat);
            reject.second.set58_text("The number of symbole doesn't match the number of group");
        } else {
            for (const auto &_sym : entrys) {
                if (_sym != "0" && _sym != "1") {
                    reject.first = true;
                    reject.second.set371_refTagId(Tag::MDEntryType);
                    reject.second.set373_sessionRejectReason(Reject::ValueOORange);
                    reject.second.set58_text("Not entry type in the group");
                    return reject;
                }
            }
        }
        return reject;
    }
}
