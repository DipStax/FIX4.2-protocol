#pragma once

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

    enum OrderRejectReason
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

    enum CancelRejectResponseTo
    {
        CancelRequest = '1',
        ReplaceRequest
    };

    enum CancelRejectReason
    {
        TooLateCancel = '0',
        UnknownOrderCancel,
        BrokerOptionCancel,
        OrderPendingCancelorReplace
    };
}