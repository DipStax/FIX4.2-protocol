using FixGuardian.Messages.Attributes;

namespace FixGuardian.Messages.Definition
{
    public class TestRequest : AMessage, IMessage
    {
        public char MsgType { get; } = '1';

        [Tag(112)]
        public string? TestReqId { get; set; } = null;
    }
}