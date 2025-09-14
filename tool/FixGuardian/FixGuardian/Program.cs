using System.Reflection;
using FixGuardian.TestFramework.Attributes;
using System;
using System.Linq;
using System.Runtime.Loader;

namespace FixGuardian
{
    internal class Program
    {
        static void Main(string[] args)
        {
            Assembly asm = Assembly.Load("FixGuardian.Test");
            IEnumerable<Type> types = asm.GetTypes().Where(type => type.GetCustomAttribute<TestSuite>() != null);

            Console.WriteLine($"Found: {types.Count()} Test Suite");
            foreach (Type type in types)
            {
                TestSuite suite = type.GetCustomAttribute<TestSuite>()!;
                IEnumerable<MethodInfo> methods = type.GetMethods().Where(method => method.GetCustomAttribute<TestCase>() != null);
                object? testSuiteInstance = Activator.CreateInstance(type);

                if (testSuiteInstance == null)
                {
                    Console.WriteLine($"Failed to create an instance of the test suite: {suite.Name}");
                    continue;
                }
                Console.WriteLine($"Found: {methods.Count()} Test case in test suite {suite.Name}");
                foreach (MethodInfo method in methods)
                {
                    bool testSucced = true;
                    TestCase testCase = method.GetCustomAttribute<TestCase>()!;
                    Console.WriteLine($"=== Running test case: {testCase.Name}");
                    try
                    {
                        method.Invoke(testSuiteInstance, null);
                    }
                    catch (Exception exception)
                    {
                        testSucced = false;
                        Console.WriteLine($"==> Test throw an exception: {exception}");
                        Console.WriteLine($"=== {testCase.Name} | Case Failed");
                    }
                    if (testSucced)
                    {
                        Console.WriteLine($"=== {testCase.Name} | Case Succed");
                    }
                }
            }
        }
    }
}
