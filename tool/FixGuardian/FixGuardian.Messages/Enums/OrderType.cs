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
        OnClose,
        [EnumMember(Value = "A")]
        LimitOnClose,
        [EnumMember(Value = "B")]
        ForexMarket,
        [EnumMember(Value = "C")]
        PrevQuoted,
        [EnumMember(Value = "D")]
        PrevIndicated,
        [EnumMember(Value = "E")]
        ForexLimit,
        [EnumMember(Value = "F")]
        ForexSwap,
        [EnumMember(Value = "G")]
        ForexPrevQuoted,
        [EnumMember(Value = "H")]
        Funari,
        [EnumMember(Value = "P")]
        Pegged
    }
}