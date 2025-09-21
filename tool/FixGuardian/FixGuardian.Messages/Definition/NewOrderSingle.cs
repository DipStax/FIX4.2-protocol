using FixGuardian.Messages.Attributes;
using FixGuardian.Messages.Enums;

namespace FixGuardian.Messages.Definition
{
    public class NewOrderSingle : AMessage, IMessage
    {
        public char MsgType { get; } = 'D';

        [Tag(11)]
        public string? ClOrdId { get; set; } = null;

        [Tag(21)]
        public HandleInstance? HeartBtInt { get; set; } = null;

        [Tag(55)]
        public string? Symbol { get; set; } = null;

        [Tag(54)]
        public TradeSide? Side { get; set; } = null;

        [OptionalTag()]
        [Tag(38)]
        public float? OrderQty { get; set; } = null;

        [Tag(40)]
        public OrderType? OrdType { get; set; } = null;

        [OptionalTag()]
        [Tag(44)]
        public float? Price { get; set; } = null;
    }
}