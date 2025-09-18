namespace FixGuardian.TestFramework.Attributes
{
    [AttributeUsage(AttributeTargets.Method, AllowMultiple = true)]
    public class TestInput : Attribute
    {
        public object Data { get; }

        public TestInput(object data)
        {
            Data = data;
        }
    }
}