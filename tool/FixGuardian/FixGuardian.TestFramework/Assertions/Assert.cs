using FixGuardian.Message.Attributes;
using System.Reflection;

namespace FixGuardian.TestFramework.Assertions
{
    public class AssertionException : Exception
    {
        public AssertionException()
        {
        }

        public AssertionException(string msg)
            : base(msg)
        {
        }

        public AssertionException(string msg, Exception inner)
            : base(msg, inner)
        {
        }
    }

    public class Assert
    {
        public static void Equal<T>(T lhs, T rhs)
        {
            IEnumerable<KeyValuePair<PropertyInfo, Tag>> props = typeof(T)
                .GetProperties(BindingFlags.Public | BindingFlags.Instance)
                .Where(prop => prop.GetCustomAttribute<Tag>() != null)
                .Select(prop => new KeyValuePair<PropertyInfo, Tag>(prop, prop.GetCustomAttribute<Tag>()!));
            bool failed = false;

            foreach (KeyValuePair<PropertyInfo, Tag> prop in props)
            {
                object? lhsobject = prop.Key.GetValue(lhs);
                object? rhsobject = prop.Key.GetValue(rhs);

                if (lhsobject is DateTime lhsDt && rhsobject is DateTime rhsDt)
                {
                    if (Math.Abs((lhsDt - rhsDt).TotalSeconds) > 1)
                    {
                        Console.WriteLine($"=> {prop.Value.TagId} | {prop.Key.Name} not equal: '{lhsDt}' != '{rhsDt}'");
                        failed = true;
                    }
                }
                else if (!Equals(lhsobject, rhsobject))
                {
                    Console.WriteLine($"=> {prop.Value.TagId} | {prop.Key.Name} not equal: '{lhsobject}' != '{rhsobject}'");
                    failed = true;
                }
            }
            if (failed)
            {
                throw new AssertionException("Test failed");
            }
        }
    }
}