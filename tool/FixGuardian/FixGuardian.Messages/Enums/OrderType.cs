using System.Runtime.Serialization;

namespace FixGuardian.Messages.Enums
{
    public enum OrderType
    {
        Market = 1,
        Limit,
        Stop,
        StopLimit,
        MarketOnClose,
        WithOrWithout,
        LimitOrBetter,
        LimitWithOrWithout,
        OnBasis,
        [EnumMember(Value = "A")]
        OnClose,
        [EnumMember(Value = "B")]
        LimitOnClose,
        [EnumMember(Value = "C")]
        ForexMarket,
        [EnumMember(Value = "D")]
        PrevQuoted,
        [EnumMember(Value = "E")]
        PrevIndicated,
        [EnumMember(Value = "F")]
        ForexLimit,
        [EnumMember(Value = "G")]
        ForexSwap,
        [EnumMember(Value = "H")]
        ForexPrevQuoted,
        [EnumMember(Value = "I")]
        Funari,
        [EnumMember(Value = "P")]
        Pegged
    }
}