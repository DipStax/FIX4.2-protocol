namespace FixGuardian.Messages.Attributes
{
    public class Tag : Attribute
    {
        public ushort TagId;

        public Tag(ushort tagId)
        {
            TagId = tagId;
        }
    }
}
