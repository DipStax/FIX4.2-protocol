namespace FixGuardian.TestFramework.Attributes
{
    [AttributeUsage(AttributeTargets.Class, AllowMultiple = true)]
    public class RequireSuite : Attribute
    {
        public string SuiteName { get; }

        public RequireSuite(string name)
        {
            SuiteName = name;
        }
    }
}