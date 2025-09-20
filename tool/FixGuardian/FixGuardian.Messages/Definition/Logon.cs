using FixGuardian.Messages.Attributes;
using FixGuardian.Messages.Enums;

namespace FixGuardian.Messages.Definition
{
    public class Logon : AMessage, IMessage
    {
        public char MsgType { get; } = 'A';

        [Tag(98)]
        public EncryptionMethod? EncryptMethod { get; set; } = null;

        [Tag(108)]
        public uint? HeartBtInt { get; set; } = null;
    }
}