#pragma once

namespace fix42
{
    enum RejectReasonSession : uint8_t
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

    enum TransactionType : uint8_t
    {
        New = '0',
        Cancel,
        Correct,
        Status
    };

    enum OrderStatus : uint8_t
    {
        NewOrder = '0',
        PartiallyFilled,
        Filled,
        DoneForDay,
        Canceled,
        Replaced,
        PendingCancel,
        Stopped,
        Rejected,
        Suspended,
        PendingNew = 'A',
        Caluclated,
        Expired,
        AcceptedBidding_Restated,
        PendingReplace
    };

    using ExecutionStatus = OrderStatus;

    enum OrderRejectReason : uint8_t
    {
        BrokerOption = '0',
        UnknowSymbol,
        ExchangeClose,
        OrderExceedLimit,
        TooLate,
        UnknownOrder,
        Duplicate,
        DuplicateVerbally,
        StaleOrder
    };

    enum Side : uint8_t
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

    enum RejectReasonBusiness : uint8_t
    {
        Other = '0',
        UnknownId,
        UnknownSecurity,
        UnsupportedMsgType,
        AppNotAvailable,
        CondReqFieldMissing
    };

    enum EncryptionMethod : uint8_t
    {
        None = '0',
        PKCS,
        DES,
        PKCSDES,
        PGPDES,
        PGPDESMD5,
        PEMDESMD5
    };

    enum HandleInstance : uint8_t
    {
        AutoPrivate = '1',
        AutoPublic,
        Manual
    };

    enum OrderType : uint8_t
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
        OnClose = 'A',
        LimitOnClose,
        ForexMarket,
        PrevQuoted,
        PrevIndicated,
        ForexLimit,
        ForexSwap,
        ForexPrevQuoted,
        Funari,
        Pegged = 'P'
    };

    enum CancelRejectResponseTo : uint8_t
    {
        CancelRequest = '1',
        ReplaceRequest
    };

    enum CancelRejectReason : uint8_t
    {
        TooLateCancel = '0',
        UnknownOrderCancel,
        BrokerOptionCancel,
        OrderPendingCancelorReplace
    };

    enum MarketDataEntryType : uint8_t
    {
        Bid = '0',
        Offer,
        Trade,
        IndexValue,
        OpeningPrice,
        ClosingPrice,
        SettlementPrice,
        TradingSessionHighPrice,
        TradingSessionLowPrice,
        TradingSessionVWAPPrice
    };

    enum SubscirptionType : uint8_t
    {
        Snapshot = '0',
        Subscirbe,
        Unsubscribe
    };

    enum MarketDataUpdateType : uint8_t
    {
        FullRefresh = '0',
        IncrementalRefresh
    };

    enum MarketDataUpAction : uint8_t
    {
        NewAction = '0',
        Change,
        Delete
    };

    enum MarketDataReqRejReason : uint8_t
    {
        UnknownSymbol = '0',
        DuplicateMDReqId,
        InsufficientBandwidth,
        InsufficientPermissions,
        UnsupportedSubReqType,
        UnsupportedDepth,
        UnsupportedUpdateType,
        UnsupportedAggBook,
        UnsupportedEntryType,
    };

    enum TickDir : uint8_t
    {
        PlusTick = '0',
        ZeroPlusTick,
        MinusTick,
        ZeroMinusTick
    };
}