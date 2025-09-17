namespace FixGuardian.Messages.Attributes
{
    public class PositionalTag : Attribute
    {
        public byte Position;

        public PositionalTag(byte position)
        {
            Position = position;
        }
    }
}
