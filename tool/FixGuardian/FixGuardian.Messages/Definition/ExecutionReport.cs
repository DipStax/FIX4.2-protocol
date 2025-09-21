using FixGuardian.Messages.Attributes;
using FixGuardian.Messages.Enums;

namespace FixGuardian.Messages.Definition
{
    public class ExecutionReport : AMessage, IMessage
    {
        public char MsgType { get; } = '8';

        [Tag(37)]
        public string? OrderID { get; set; } = null;

        [Tag(17)]
        public string? ExecId { get; set; } = null;

        [Tag(20)]
        public TransactionType? ExecTransType { get; set; } = null;

        [Tag(150)]
        public ExecutionType? ExecType { get; set; } = null;

        [Tag(39)]
        public OrderStatus? OrdStatus { get; set; } = null;

        [Tag(55)]
        public string? Symbol { get; set; } = null;

        [Tag(54)]
        public TradeSide? Side { get; set; } = null;

        [OptionalTag()]
        [Tag(38)]
        public float? OrderQty { get; set; } = null;

        [OptionalTag()]
        [Tag(40)]
        public OrderType? OrdType { get; set; } = null;

        [OptionalTag()]
        [Tag(44)]
        public float? Price { get; set; } = null;

        [Tag(151)]
        public float? LeavesQty { get; set; } = null;

        [Tag(14)]
        public float? CumQty { get; set; } = null;

        [Tag(6)]
        public float? AvgPx { get; set; } = null;
    }
}