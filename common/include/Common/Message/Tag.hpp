#pragma once

#include <utility>

#include "Common/Message/Reject.hpp"
#include "Common/Core/Order.hpp"

#define UNUSED __attribute__((unused))

namespace fix
{
    /// @brief Structure containing all the [Tag](https://www.onixs.biz/fix-dictionary/4.2/fields_by_tag.html) of the FIX protocol.
    namespace Tag
    {
        constexpr const char AvgPx[] = "6";                     ///< Tag [6](https://www.onixs.biz/fix-dictionary/4.2/tagNum_6.html).
        constexpr const char BeginString[] = "8";               ///< Tag [8](https://www.onixs.biz/fix-dictionary/4.2/tagNum_8.html).
        constexpr const char BodyLength[] = "9";                ///< Tag [9](https://www.onixs.biz/fix-dictionary/4.2/tagNum_9.html).
        constexpr const char CheckSum[] = "10";                 ///< Tag [10](https://www.onixs.biz/fix-dictionary/4.2/tagNum_10.html).
        constexpr const char ClOrdID[] = "11";                  ///< Tag [11](https://www.onixs.biz/fix-dictionary/4.2/tagNum_11.html).
        constexpr const char CumQty[] = "14";                   ///< Tag [14](https://www.onixs.biz/fix-dictionary/4.2/tagNum_14.html).
        constexpr const char ExecId[] = "17";                   ///< Tag [17](https://www.onixs.biz/fix-dictionary/4.2/tagNum_17.html).
        constexpr const char ExecTransType[] = "20";            ///< Tag [20](https://www.onixs.biz/fix-dictionary/4.2/tagNum_20.html).
        constexpr const char HandlInst[] = "21";                ///< Tag [21](https://www.onixs.biz/fix-dictionary/4.2/tagNum_21.html).
        constexpr const char MsqSeqNum[] = "34";                ///< Tag [34](https://www.onixs.biz/fix-dictionary/4.2/tagNum_34.html).
        constexpr const char MsgType[] = "35";                  ///< Tag [35](https://www.onixs.biz/fix-dictionary/4.2/tagNum_35.html).
        constexpr const char OrderID[] = "37";                  ///< Tag [37](https://www.onixs.biz/fix-dictionary/4.2/tagNum_37.html).
        constexpr const char OrderQty[] = "38";                 ///< Tag [38](https://www.onixs.biz/fix-dictionary/4.2/tagNum_38.html).
        constexpr const char OrdStatus[] = "39";                ///< Tag [39](https://www.onixs.biz/fix-dictionary/4.2/tagNum_39.html).
        constexpr const char OrdType[] = "40";                  ///< Tag [40](https://www.onixs.biz/fix-dictionary/4.2/tagNum_40.html).
        constexpr const char OrigClOrdID[] = "41";              ///< Tag [41](https://www.onixs.biz/fix-dictionary/4.2/tagNum_41.html).
        constexpr const char Price[] = "44";                    ///< Tag [44](https://www.onixs.biz/fix-dictionary/4.2/tagNum_44.html).
        constexpr const char RefSeqNum[] = "45";                ///< Tag [45](https://www.onixs.biz/fix-dictionary/4.2/tagNum_45.html).
        constexpr const char SenderCompId[] = "49";             ///< Tag [49](https://www.onixs.biz/fix-dictionary/4.2/tagNum_49.html).
        constexpr const char SendingTime[] = "52";              ///< Tag [52](https://www.onixs.biz/fix-dictionary/4.2/tagNum_52.html).
        constexpr const char Side[] = "54";                     ///< Tag [54](https://www.onixs.biz/fix-dictionary/4.2/tagNum_54.html).
        constexpr const char Symbol[] = "55";                   ///< Tag [55](https://www.onixs.biz/fix-dictionary/4.2/tagNum_55.html).
        constexpr const char TargetCompId[] = "56";             ///< Tag [56](https://www.onixs.biz/fix-dictionary/4.2/tagNum_56.html).
        constexpr const char Text[] = "58";                     ///< Tag [58](https://www.onixs.biz/fix-dictionary/4.2/tagNum_58.html).
        constexpr const char TransactTime[] = "60";             ///< Tag [60](https://www.onixs.biz/fix-dictionary/4.2/tagNum_60.html).
        constexpr const char EncryptMethod[] = "98";            ///< Tag [98](https://www.onixs.biz/fix-dictionary/4.2/tagNum_98.html).
        constexpr const char HearBtInt[] = "108";               ///< Tag [108](https://www.onixs.biz/fix-dictionary/4.2/tagNum_108.html).
        constexpr const char MinQty[] = "110";                  ///< Tag [110](https://www.onixs.biz/fix-dictionary/4.2/tagNum_110.html).
        constexpr const char TestReqId[] = "112";               ///< Tag [112](https://www.onixs.biz/fix-dictionary/4.2/tagNum_112.html).
        constexpr const char NoRelatedSym[] = "146";            ///< Tag [146](https://www.onixs.biz/fix-dictionary/4.2/tagNum_146.html).
        constexpr const char ExecType[] = "150";                ///< Tag [150](https://www.onixs.biz/fix-dictionary/4.2/tagNum_150.html).
        constexpr const char LeavesQty[] = "151";               ///< Tag [151](https://www.onixs.biz/fix-dictionary/4.2/tagNum_151.html).
        constexpr const char MDReqID[] = "262";                 ///< Tag [262](https://www.onixs.biz/fix-dictionary/4.2/tagNum_262.html).
        constexpr const char SubscriptionRequestType[] = "263"; ///< Tag [263](https://www.onixs.biz/fix-dictionary/4.2/tagNum_263.html).
        constexpr const char MarketDepth[] = "264";             ///< Tag [264](https://www.onixs.biz/fix-dictionary/4.2/tagNum_264.html).
        constexpr const char MDUpdateType[] = "265";            ///< Tag [265](https://www.onixs.biz/fix-dictionary/4.2/tagNum_265.html).
        constexpr const char NoMDEntryTypes[] = "267";          ///< Tag [267](https://www.onixs.biz/fix-dictionary/4.2/tagNum_267.html).
        constexpr const char NoMDEntries[] = "268";             ///< Tag [268](https://www.onixs.biz/fix-dictionary/4.2/tagNum_268.html).
        constexpr const char MDEntryType[] = "269";             ///< Tag [269](https://www.onixs.biz/fix-dictionary/4.2/tagNum_269.html).
        constexpr const char MDEntryPx[] = "270";               ///< Tag [270](https://www.onixs.biz/fix-dictionary/4.2/tagNum_270.html).
        constexpr const char MDUpdateAction[] = "279";          ///< Tag [279](https://www.onixs.biz/fix-dictionary/4.2/tagNum_279.html).
        constexpr const char MDentryRefID[] = "280";            ///< Tag [280](https://www.onixs.biz/fix-dictionary/4.2/tagNum_280.html).
        constexpr const char RefTagId[] = "371";                ///< Tag [371](https://www.onixs.biz/fix-dictionary/4.2/tagNum_371.html).
        constexpr const char RefMsgType[] = "352";              ///< Tag [352](https://www.onixs.biz/fix-dictionary/4.2/tagNum_352.html).
        constexpr const char SessionRejectReason[] = "373";     ///< Tag [373](https://www.onixs.biz/fix-dictionary/4.2/tagNum_373.html).
    };

    /// @brief Verify the value of all the fix::Tag passed as template.
    /// @tparam T fix::Tag to verify in the function.
    /// @tparam ...Ts Remaining of fix::Tag to check recursivly.
    /// @param _msg Map of the message.
    /// @return If the first element is true then second is set, otherwise it rigly formated.
    template<const char *T, const char *...Ts>
    std::pair<bool, Reject> verify_all(Serializer::AnonMessage &_msg);


    /// @brief Verify base function, it need to have a specialisation to compile.
    /// @tparam T Tag of to verify
    /// @param  _ Unknown parameters.
    /// @return Assert error
    template<const char *T>
    std::pair<bool, Reject> verify(const std::string &);
    /// @brief Verify base function, it need to have a specialisation to compile.
    /// @tparam T Tag of to verify
    /// @param  _ Unknown parameters.
    /// @param  _ Unknown parameters.
    /// @return Assert error
    template<const char *T>
    std::pair<bool, Reject> verify(const std::string &, const std::string &);
    /// @brief Verify base function, it need to have a specialisation to compile.
    /// @tparam T Tag of to verify
    /// @param  _ Unknown parameters.
    /// @param  _ Unknown parameters.
    /// @return Assert error
    template<const char *T>
    std::pair<bool, Reject> verify(const std::string &, size_t);

    /// @brief Verify the value of Tag::BeginString.
    /// @param _value value of the tag.
    /// @return If the first element is true then second is set, otherwise it rigly formated.
    template<>
    UNUSED std::pair<bool, Reject> verify<Tag::BeginString>(const std::string &_value);
    /// @brief Verify the value of Tag::BodyLength.
    /// @param _value value of the tag.
    /// @param _len calculated body length.
    /// @return If the first element is true then second is set, otherwise it rigly formated.
    template<>
    UNUSED std::pair<bool, Reject> verify<Tag::BodyLength>(const std::string &_value, size_t _len);
    /// @brief Verify the value of Tag::CheckSum.
    /// @param _value value of the tag.
    /// @param _len calculated checksum.
    /// @return If the first element is true then second is set, otherwise it rigly formated.
    template<>
    UNUSED std::pair<bool, Reject> verify<Tag::CheckSum>(const std::string &_value, const std::string &_checksum);
    /// @brief Verify the value of Tag::ClOrdID.
    /// @param _value value of the tag.
    /// @return If the first element is true then second is set, otherwise it rigly formated.
    template<>
    UNUSED std::pair<bool, Reject> verify<Tag::ClOrdID>(const std::string &_value);
    /// @brief Verify the value of Tag::HandlInst.
    /// @param _value value of the tag.
    /// @return If the first element is true then second is set, otherwise it rigly formated.
    template<>
    UNUSED std::pair<bool, Reject> verify<Tag::HandlInst>(const std::string &_value);
    /// @brief Verify the value of Tag::MsqSeqNum.
    /// @param _value value of the tag.
    /// @param _seqnum actual sequence number.
    /// @return If the first element is true then second is set, otherwise it rigly formated.
    template<>
    UNUSED std::pair<bool, Reject> verify<Tag::MsqSeqNum>(const std::string &_value, size_t _seqnum);
    /// @brief Verify the value of Tag::MsgType.
    /// @param _value value of the tag.
    /// @return If the first element is true then second is set, otherwise it rigly formated.
    template<>
    UNUSED std::pair<bool, Reject> verify<Tag::MsgType>(const std::string &_value);
    /// @brief Verify the value of Tag::OrderQty.
    /// @param _value value of the tag.
    /// @return If the first element is true then second is set, otherwise it rigly formated.
    template<>
    UNUSED std::pair<bool, Reject> verify<Tag::OrderQty>(const std::string &_value);
    /// @brief Verify the value of Tag::OrdType.
    /// @param _value value of the tag.
    /// @return If the first element is true then second is set, otherwise it rigly formated.
    template<>
    UNUSED std::pair<bool, Reject> verify<Tag::OrdType>(const std::string &_value);
    /// @brief Verify the value of Tag::OrigClOrdID.
    /// @param _value value of the tag.
    /// @return If the first element is true then second is set, otherwise it rigly formated.
    template<>
    UNUSED std::pair<bool, Reject> verify<Tag::OrigClOrdID>(const std::string &_value);
    /// @brief Verify the value of Tag::Price.
    /// @param _value value of the tag.
    /// @return If the first element is true then second is set, otherwise it rigly formated.
    template<>
    UNUSED std::pair<bool, Reject> verify<Tag::Price>(const std::string &_value);
    /// @brief Verify the value of Tag::SenderCompId.
    /// @param _value value of the tag.
    /// @param _sender valide sender Id.
    /// @return If the first element is true then second is set, otherwise it rigly formated.
    template<>
    UNUSED std::pair<bool, Reject> verify<Tag::SenderCompId>(const std::string &_value, const UserId &_sender);
    /// @brief Verify the value of Tag::SendingTime.
    /// @param _value value of the tag.
    /// @return If the first element is true then second is set, otherwise it rigly formated.
    template<>
    [[deprecated("Not implemented yet")]]
    UNUSED std::pair<bool, Reject> verify<Tag::SendingTime>(const std::string &_value);
    /// @brief Verify the value of Tag::Side.
    /// @param _value value of the tag.
    /// @return If the first element is true then second is set, otherwise it rigly formated.
    template<>
    UNUSED std::pair<bool, Reject> verify<Tag::Side>(const std::string &_value);
    /// @brief Verify the value of Tag::TargetCompId.
    /// @param _value value of the tag.
    /// @param _target valide target Id.
    /// @return If the first element is true then second is set, otherwise it rigly formated.
    template<>
    UNUSED std::pair<bool, Reject> verify<Tag::TargetCompId>(const std::string &_value, const UserId &_target);
    /// @brief Verify the value of Tag::Symbol.
    /// @param _value value of the tag.
    /// @return If the first element is true then second is set, otherwise it rigly formated.
    template<>
    UNUSED std::pair<bool, Reject> verify<Tag::Symbol>(const std::string &_value);
    /// @brief Verify the value of Tag::Symbol inside a group.
    /// @param _value value of the tag.
    /// @param _n Number of value.
    /// @return If the first element is true then second is set, otherwise it rigly formated.
    template<>
    UNUSED std::pair<bool, Reject> verify<Tag::Symbol>(const std::string &_value, size_t _n);
    /// @brief Verify the value of Tag::TransactTime.
    /// @param _value value of the tag.
    /// @return If the first element is true then second is set, otherwise it rigly formated.
    template<>
    [[deprecated("Not implemented yet")]]
    UNUSED std::pair<bool, Reject> verify<Tag::TransactTime>(const std::string &_value);
    /// @brief Verify the value of Tag::EncryptMethod.
    /// @param _value value of the tag.
    /// @return If the first element is true then second is set, otherwise it rigly formated.
    template<>
    UNUSED std::pair<bool, Reject> verify<Tag::EncryptMethod>(const std::string &_value);
    /// @brief Verify the value of Tag::HearBtInt.
    /// @param _value value of the tag.
    /// @return If the first element is true then second is set, otherwise it rigly formated.
    template<>
    UNUSED std::pair<bool, Reject> verify<Tag::HearBtInt>(const std::string &_value);
    /// @brief Verify the value of Tag::NoRelatedSym.
    /// @param _value value of the tag.
    /// @return If the first element is true then second is set, otherwise it rigly formated.
    template<>
    UNUSED std::pair<bool, Reject> verify<Tag::NoRelatedSym>(const std::string &_value);
    /// @brief Verify the value of Tag::SubscriptionRequestType.
    /// @param _value value of the tag.
    /// @return If the first element is true then second is set, otherwise it rigly formated.
    template<>
    UNUSED std::pair<bool, Reject> verify<Tag::SubscriptionRequestType>(const std::string &_value);
    /// @brief Verify the value of Tag::MarketDepth.
    /// @param _value value of the tag.
    /// @return If the first element is true then second is set, otherwise it rigly formated.
    template<>
    UNUSED std::pair<bool, Reject> verify<Tag::MarketDepth>(const std::string &_value);
    /// @brief Verify the value of Tag::NoMDEntryTypes.
    /// @param _value value of the tag.
    /// @return If the first element is true then second is set, otherwise it rigly formated.
    template<>
    UNUSED std::pair<bool, Reject> verify<Tag::NoMDEntryTypes>(const std::string &_value);
    /// @brief Verify the value of Tag::NoMDEntryTypes.
    /// @param _value value of the tag.
    /// @param _n Number of value.
    /// @return If the first element is true then second is set, otherwise it rigly formated.
    template<>
    UNUSED std::pair<bool, Reject> verify<Tag::MDEntryType>(const std::string &_value, size_t _n);

#pragma GCC diagnostic pop

}

#include "Common/Message/Tag.inl"