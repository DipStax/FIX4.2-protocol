using FixGuardian.Messages.Attributes;
using FixGuardian.Messages.Enums;

namespace FixGuardian.Messages.Definition
{
    public class OrderCancelRequest : AMessage, IMessage
    {
        public char MsgType { get; } = 'F';

        [Tag(41)]
        public string? OrigClOrdID { get; set; } = null;

        [OptionalTag]
        [Tag(37)]
        public string? OrderID { get; set; } = null;

        [Tag(11)]
        public string? ClOrdId { get; set; } = null;

        [Tag(55)]
        public string? Symbol { get; set; } = null;

        [Tag(54)]
        public TradeSide? Side { get; set; } = null;

        [Tag(60)]
        public DateTime? TransactTime { get; set; } = null;

        [OptionalTag]
        [Tag(38)]
        public float? OrderQty { get; set; } = null;
    }
}