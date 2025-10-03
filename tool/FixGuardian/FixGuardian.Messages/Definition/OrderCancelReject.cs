using FixGuardian.Messages.Attributes;
using FixGuardian.Messages.Enums;

namespace FixGuardian.Messages.Definition
{
    public class OrderCancelReject : AMessage, IMessage
    {
        public char MsgType { get; } = '9';

        [Tag(37)]
        public string? OrderId { get; set; } = null;

        [Tag(41)]
        public string? OrigClOrdId { get; set; } = null;

        [Tag(39)]
        public OrderStatus? OrdStatus { get; set; } = null;

        [Tag(434)]
        public CancelRejectResponseTo? CxlRejResponseTo { get; set; } = null;

        [OptionalTag]
        [Tag(102)]
        public CancelRejectReason? CxlRejReason { get; set; } = null;

        [OptionalTag]
        [Tag(58)]
        public string? Text { get; set; } = null;
    }
}