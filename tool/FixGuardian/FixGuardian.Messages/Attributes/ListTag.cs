namespace FixGuardian.Messages.Attributes
{
    public class ListTag : Attribute
    {
        public uint NoTag { get; }

        public ListTag(uint noTag)
        {
            NoTag = noTag;
        }
    }
}