namespace FixGuardian.TestFramework.Attributes
{
    public class TestSuite : Attribute
    {
        public string Name { get; private set; }

        public TestSuite(string name)
        {
            Name = name;
        }
    }
}

