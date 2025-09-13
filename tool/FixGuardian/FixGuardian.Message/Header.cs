using FixGuardian.Message.Attributes;

namespace FixGuardian.Message
{
    public class Header
    {
        [PositionalTag(1)]
        [Tag(8)]
        public string? BeginString { get; set; } = null;

        [PositionalTag(2)]
        [Tag(9)]
        public UInt32? BodyLength { get; set; } = null;

        [PositionalTag(3)]
        [Tag(35)]
        public char? MsgType { get; set; } = null;

        [Tag(49)]
        public string? SenderCompId { get; set; } = null;

        [Tag(56)]
        public string? TargetCompId { get; set; } = null;

        [Tag(34)]
        public UInt32? MsgSeqNum { get; set; } = null;

        [Tag(52)]
        public DateTime? SendingTime { get; set; } = null;
    }
}
