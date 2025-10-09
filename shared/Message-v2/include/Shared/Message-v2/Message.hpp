#pragma once

#include <cstdint>
#include <chrono>

#include "Shared/Core/Core.hpp"

#include "Shared/Message-v2/FixConvertor.hpp"
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

    namespace list
    {
        using MDEntriesSnapshot = fix::List<
                fix::TagNo<tag::NoMDEntries, false>,
                fix::Tag<tag::MDEntryType, MarketDataEntryType>,
                fix::Tag<tag::MDEntryPx, Price>,
                fix::Tag<tag::MDEntrySize, std::optional<Quantity>>,
                // fix::Tag<tag::MDEntryDate, std::optional<std::chrono::time_point<std::chrono::system_clock>>>,
                // fix::Tag<tag::MDEntryTime, std::optional<std::chrono::time_point<std::chrono::system_clock>>>,
                fix::Tag<tag::TickDirection, std::optional<TickDir>>
            >;
    }

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
            fix::Tag<tag::ClOrdID, std::optional<std::string>>,
            fix::Tag<tag::ExecId, std::string>,
            fix::Tag<tag::ExecTransType, TransactionType>,
            fix::Tag<tag::ExecType, ExecutionStatus>,
            fix::Tag<tag::OrdStatus, OrderStatus>,
            fix::Tag<tag::OrdRejReason, std::optional<OrderRejectReason>>,
            fix::Tag<tag::Symbol, std::string>,
            fix::Tag<tag::Side, Side>,
            fix::Tag<tag::OrderQty, std::optional<Quantity>>,
            fix::Tag<tag::OrdType, std::optional<OrderType>>,
            fix::Tag<tag::Price, std::optional<Price>>,
            fix::Tag<tag::LeavesQty, Quantity>,
            fix::Tag<tag::LastShares, std::optional<Quantity>>,
            fix::Tag<tag::LastPx, std::optional<Quantity>>,
            fix::Tag<tag::CumQty, Quantity>,
            fix::Tag<tag::AvgPx, Price>,
            fix::Tag<tag::Text, std::optional<std::string>>
        >;

        using OrderCancelReject = fix::Message<'9',
            fix::Tag<tag::OrderID, std::string>,
            fix::Tag<tag::OrigClOrdID, std::string>,
            fix::Tag<tag::OrdStatus, OrderStatus>,
            fix::Tag<tag::CxlRejResponseTo, CancelRejectResponseTo>,
            fix::Tag<tag::CxlRejReason, std::optional<CancelRejectReason>>,
            fix::Tag<tag::Text, std::optional<std::string>>
        >;

        using BusinessReject = fix::Message<'j',
            fix::Tag<tag::RefSeqNum, std::optional<uint32_t>>,
            fix::Tag<tag::RefMsgType, char>,
            fix::Tag<tag::BusinessRejectRefId, std::optional<std::string>>,
            fix::Tag<tag::BusinessRejectReason, RejectReasonBusiness>,
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
            fix::Tag<tag::OrderQty, std::optional<Quantity>>,
            fix::Tag<tag::OrdType, OrderType>,
            fix::Tag<tag::Price, std::optional<Price>>
        >;

        using OrderCancelRequest = fix::Message<'F',
            fix::Tag<tag::OrigClOrdID, std::string>,
            fix::Tag<tag::OrderID, std::optional<std::string>>,
            fix::Tag<tag::ClOrdID, std::string>,
            fix::Tag<tag::Symbol, std::string>,
            fix::Tag<tag::Side, Side>,
            fix::Tag<tag::TransactTime, std::chrono::time_point<std::chrono::system_clock>>,
            fix::Tag<tag::OrderQty, std::optional<Quantity>>
        >;

        using MarketDataRequest = fix::Message<'V',
            fix::TagList<
                fix::List<
                    fix::TagNo<tag::NoMDEntryTypes, false>,
                    fix::Tag<tag::MDEntryType, MarketDataEntryType>
                >,
                fix::List<
                    fix::TagNo<tag::NoRelatedSym, false>,
                    fix::Tag<tag::Symbol, std::string>
                >
            >,
            fix::Tag<tag::MDReqID, std::string>,
            fix::Tag<tag::SubscriptionRequestType, SubscirptionType>,
            fix::Tag<tag::MarketDepth, uint32_t>,
            fix::Tag<tag::MDUpdateType, std::optional<MarketDataUpdateType>>
        >;

        using MarketDataSnapshotFullRefresh = fix::Message<'W',
            fix::TagList<
                list::MDEntriesSnapshot
            >,
            fix::Tag<tag::MDReqID, std::optional<std::string>>,
            fix::Tag<tag::Symbol, std::string>
        >;

        using MarketDataIncrementalRefresh = fix::Message<'X',
            fix::TagList<
                fix::List<
                    fix::TagNo<tag::NoMDEntries, false>,
                    fix::Tag<tag::MDUpdateAction, MarketDataUpAction>,
                    fix::Tag<tag::MDEntryType, std::optional<MarketDataEntryType>>,
                    fix::Tag<tag::MDEntryRefID, std::optional<std::string>>,
                    fix::Tag<tag::Symbol, std::optional<std::string>>,
                    fix::Tag<tag::MDEntryPx, std::optional<Price>>,
                    fix::Tag<tag::MDEntrySize, std::optional<Quantity>>,
                    // fix::Tag<tag::MDEntryDate, std::optional<std::chrono::time_point<std::chrono::system_clock>>>,
                    // fix::Tag<tag::MDEntryTime, std::optional<std::chrono::time_point<std::chrono::system_clock>>>,
                    fix::Tag<tag::TickDirection, std::optional<TickDir>>
                >
            >,
            fix::Tag<tag::MDReqID, std::optional<std::string>>
        >;


        using MarketDataRequestReject = fix::Message<'Y',
            fix::Tag<tag::MDReqID, std::string>,
            fix::Tag<tag::MDReqRejReason, std::optional<MarketDataReqRejReason>>,
            fix::Tag<tag::Text, std::optional<std::string>>
        >;
    }
}