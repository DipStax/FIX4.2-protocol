using FixGuardian.Messages.Attributes;
using FixGuardian.Messages.Enums;

namespace FixGuardian.Messages.Definition
{
    public class BusinessMessageReject : AMessage, IMessage
    {
        public char MsgType { get; } = 'j';

        [OptionalTag]
        [Tag(45)]
        public uint? RefSeqNum { get; set; } = null;

        [Tag(372)]
        public char? RefMsgType { get; set; } = null;

        [OptionalTag]
        [Tag(379)]
        public string? BusinessRejectRefId { get; set; } = null;

        [Tag(380)]
        public RejectReasonBusiness? BusinessRejectReason { get; set; } = null;

        [OptionalTag]
        [Tag(58)]
        public string? Text { get; set; } = null;
    }
}