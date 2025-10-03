namespace FixGuardian.Messages.Enums
{
    public enum CancelRejectReason
    {
        TooLate = 0,
        UnknownOrder,
        BrokerOption,
        OrderPendingCancelorReplace
    }
}