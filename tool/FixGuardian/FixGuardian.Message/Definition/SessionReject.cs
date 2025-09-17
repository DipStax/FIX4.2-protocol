using FixGuardian.Messages.Attributes;
using FixGuardian.Messages.enums;

namespace FixGuardian.Messages.Definition
{
    public class SessionReject : AMessage
    {
        [Tag(45)]
        public uint? RefSeqNum { get; set; } = null;

        [OptionalTag()]
        [Tag(371)]
        public ushort? RefTagId { get; set; } = null;

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