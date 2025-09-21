using System.Runtime.Serialization;

namespace FixGuardian.Messages.Enums
{
    public enum OrderStatus
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
        [EnumMember(Value = "A")]
        PendingNew,
        [EnumMember(Value = "B")]
        Caluclated,
        [EnumMember(Value = "C")]
        Expired,
        [EnumMember(Value = "D")]
        AcceptedBidding,
        [EnumMember(Value = "E")]
        PendingReplace
    };
}