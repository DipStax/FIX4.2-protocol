namespace FixGuardian.Messages.Exceptions
{
    public class FixEncodeException : Exception
    {
        public uint RefTagId { get; private set; }

        public FixEncodeException(string msg, uint tagid)
            : base(msg)
        {
            RefTagId = tagid;
        }
    }
}
