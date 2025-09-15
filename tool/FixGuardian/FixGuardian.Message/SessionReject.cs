using FixGuardian.Message.Attributes;
using FixGuardian.Message.enums;

namespace FixGuardian.Message
{
    public class SessionReject
    {
        [Tag(45)]
        public UInt32? RefSeqNum { get; set; } = null;

        [OptionalTag()]
        [Tag(371)]
        public UInt16? RefTagId { get; set; } = null;

        [OptionalTag()]
        [Tag(372)]
        public char? RefMsgType { get; set; } = null;

        [OptionalTag()]
        [Tag(373)]
        public RejectReasonSession? SessionRejectReason { get; set; } = null;

        [OptionalTag()]
        [Tag(58)]
        public string? Text { get; set; } = null;
    }
}