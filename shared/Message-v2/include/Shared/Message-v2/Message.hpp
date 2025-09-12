#pragma once

#include <cstdint>
#include <chrono>

#include "Shared/Message-v2/TagConvertor.hpp"
#include "Shared/Message-v2/Tag.hpp"

#include "FIX-Message/Header.hpp"
#include "FIX-Message/Message.hpp"

namespace fix42
{
    using Header = fix::Header<
        fix::PositionalTag<
            fix::Tag<tag::BeginString, std::string>,
            fix::Tag<tag::BodyLength, uint32_t>,
            fix::Tag<tag::MsgType, char>
        >,
        fix::Tag<tag::SenderCompId, std::string>,
        fix::Tag<tag::TargetCompId, std::string>,
        fix::Tag<tag::MsgSeqNum, uint32_t>,
        fix::Tag<tag::SendingTime, std::chrono::time_point<std::chrono::system_clock>>
    >;

    namespace msg
    {
        using HeartBeat = fix::Message<'0',
            fix::Tag<tag::TestReqId, std::optional<std::string>>
        >;

        using TestRequest = fix::Message<'1',
            fix::Tag<tag::TestReqId, std::string>
        >;

        using SessionReject = fix::Message<'3',
            fix::Tag<tag::RefSeqNum, uint32_t>,
            fix::Tag<tag::RefTagId, std::optional<uint16_t>>,
            fix::Tag<tag::RefMsgType, std::optional<char>>,
            fix::Tag<tag::SessionRejectReason, std::optional<RejectReasonSession>>,
            fix::Tag<tag::Text, std::optional<std::string>>
        >;

        using Logout = fix::Message<'5'>;

        using ExecutionReport = fix::Message<'8',
            fix::Tag<tag::OrderID, std::string>,
            fix::Tag<tag::ExecId, std::string>,
            fix::Tag<tag::ExecTransType, uint8_t>,
            fix::Tag<tag::OrdStatus, OrderStatus>,
            fix::Tag<tag::Symbol, std::string>,
            fix::Tag<tag::Side, Side>,
            fix::Tag<tag::LeavesQty, float>,
            fix::Tag<tag::CumQty, float>,
            fix::Tag<tag::AvgPx, float>
        >;

        using BusinessReject = fix::Message<'j',
            fix::Tag<tag::RefSeqNum, std::optional<uint32_t>>,
            fix::Tag<tag::RefMsgType, char>,
            fix::Tag<tag::SessionRejectReason, std::optional<std::string>>,
            fix::Tag<tag::BusinessRejectRefId, RejectReasonBusiness>,
            fix::Tag<tag::Text, std::optional<std::string>>
        >;

        using Logon = fix::Message<'A',
            fix::Tag<tag::EncryptMethod, EncryptionMethod>,
            fix::Tag<tag::HeartBtInt, uint32_t>
        >;

        using NewOrderSingle = fix::Message<'D',
            fix::Tag<tag::ClOrdID, std::string>,
            fix::Tag<tag::HandlInst, HandleInstance>,
            fix::Tag<tag::Symbol, std::string>,
            fix::Tag<tag::Side, Side>,
            fix::Tag<tag::OrdType, OrderType>,
            fix::Tag<tag::Price, float>
        >;
    }
}