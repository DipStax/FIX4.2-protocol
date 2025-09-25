using FixGuardian.Messages.Attributes;

namespace FixGuardian.Messages.Definition
{
    public class HeartBeat : AMessage, IMessage
    {
        public char MsgType { get; } = '0';

        [OptionalTag()]
        [Tag(112)]
        public string? TestReqId { get; set; } = null;
    }
}