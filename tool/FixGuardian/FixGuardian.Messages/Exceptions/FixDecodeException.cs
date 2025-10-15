namespace FixGuardian.Messages.Exceptions
{
    public class FixDecodeException : Exception
    {
        public FixDecodeException(string msg, Exception inner)
            : base(msg, inner)
        {
        }

        public FixDecodeException(string msg)
            : base(msg)
        {
        }
    }
}
