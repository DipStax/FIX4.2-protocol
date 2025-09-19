namespace FixGuardian.Messages.Definition
{
    public class Logout : AMessage, IMessage
    {
        public char MsgType { get; } = '5';
    }
}