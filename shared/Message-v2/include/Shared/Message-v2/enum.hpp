#pragma once

#include <optional>

#include "FIX-Message/RejectError.hpp"

namespace fix42
{
    enum RejectReasonSession
    {
        InvalidTagNum = 1,
        RequiredTagMissing,
        TagNotDefine,
        UndefineTag,
        TagWithoutValue,
        ValueOutOfRange,
        IncorrectDataFormat,
        SignatureProblem,
        CompIDProblem,
        SendingTimeAccuracy,
        InvalidMsgType
    };

    enum TransactionType
    {
        New = '0',
        Cancel,
        Correct,
        Status
    };

    enum OrderStatus
    {
        NewOrder = '0',
        PartiallyFilled,
        Filled,
        DoneForDay,
        Canceled,
        Replaced,
        PedningCancel,
        Stopped,
        Rejected,
        Suspended,
        PendingNew = 'A',
        Caluclated,
        Expired,
        AcceptedBidding,
        PendingReplace
    };

    enum Side
    {
        Buy = '1',
        Sell,
        BuyMinus,
        SellPlus,
        SellShort,
        SellShortExempt,
        Undisclosed,
        Cross,
        CrossShort
    };

    enum RejectReasonBusiness
    {
        Other = '0',
        UnknownId,
        UnknownSecurity,
        UnsupportedMsgType,
        AppNotAvailable,
        CondReqFieldMissing
    };

    enum EncryptionMethod
    {
        None = '0',
        PKCS,
        DES,
        PKCSDES,
        PGPDES,
        PGPDESMD5,
        PEMDESMD5
    };

    enum HandleInstance
    {
        AutoPrivate = '1',
        AutoPublic,
        Manual
    };

    enum OrderType
    {
        Market = '1',
        Limit,
        Stop,
        StopLimit,
        MarketOnClose,
        WithOrWithout,
        LimitOrBetter,
        LimitWithOrWithout,
        OnBasis,
        OnClose,
        LimitOnClose = 'A',
        ForexMarket,
        PrevQuoted,
        PrevIndicated,
        ForexLimit,
        ForexSwap,
        ForexPrevQuoted,
        Funari,
        Pegged = 'P'
    };
}

std::optional<fix::RejectError> TagConvertor(const std::string &_value, fix42::RejectReasonSession &_out);
std::optional<fix::RejectError> TagConvertor(const std::string &_value, fix42::TransactionType &_out);
std::optional<fix::RejectError> TagConvertor(const std::string &_value, fix42::OrderStatus &_out);
std::optional<fix::RejectError> TagConvertor(const std::string &_value, fix42::Side &_out);
std::optional<fix::RejectError> TagConvertor(const std::string &_value, fix42::RejectReasonBusiness &_out);
std::optional<fix::RejectError> TagConvertor(const std::string &_value, fix42::EncryptionMethod &_out);
std::optional<fix::RejectError> TagConvertor(const std::string &_value, fix42::HandleInstance &_out);
std::optional<fix::RejectError> TagConvertor(const std::string &_value, fix42::OrderType &_out);