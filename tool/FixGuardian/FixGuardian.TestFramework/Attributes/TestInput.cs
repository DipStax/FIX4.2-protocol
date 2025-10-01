namespace FixGuardian.TestFramework.Attributes
{
    [AttributeUsage(AttributeTargets.Method, AllowMultiple = true)]
    public class TestInput : Attribute
    {
        public object[] Data { get; }

        public TestInput(params object[] data)
        {
            Data = data;
        }
    }
}