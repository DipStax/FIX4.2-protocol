namespace FixGuardian.TestFramework.Attributes
{
    [AttributeUsage(AttributeTargets.Method, AllowMultiple = true)]
    public class TestInput<T> : Attribute
    {
        public T Data { get; }

        public TestInput(T data)
        {
            Data = data;
        }
    }
}