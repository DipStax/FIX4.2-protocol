namespace FixGuardian.Message.Attributes
{
    public class Tag : Attribute
    {
        public UInt32 TagId;

        public Tag(UInt32 tagId)
        {
            TagId = tagId;
        }
    }
}
