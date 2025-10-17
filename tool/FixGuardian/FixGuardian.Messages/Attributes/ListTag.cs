namespace FixGuardian.Messages.Attributes
{
    public class ListTag : Attribute
    {
        public ushort NoTag { get; }

        public ListTag(ushort noTag)
        {
            NoTag = noTag;
        }
    }
}