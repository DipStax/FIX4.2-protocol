namespace FixGuardian.TestFramework.Attributes
{
    public class TestCase : Attribute
    {
        public string Name { get; private set; }

        public TestCase(string name)
        {
            Name = name;
        }
    }
}

