namespace FixGuardian.Messages.Enums
{
    public enum OrderRejectReason
    {
        BrokerOption = 0,
        UnknowSymbol,
        ExchangeClose,
        OrderExceedLimit,
        TooLate,
        UnknownOrder,
        Duplicate,
        DuplicateVerbally,
        StaleOrder
    };

}