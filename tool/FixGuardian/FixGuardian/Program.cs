using System.Reflection;
using FixGuardian.TestFramework.Attributes;
using System;
using System.Runtime.Loader;
using FixGuardian.TestFramework.Assertions;
using System.Security.Cryptography;

namespace FixGuardian
{
    internal class Program
    {
        static void Main(string[] args)
        {
            IEnumerable<Type> types = AppDomain.CurrentDomain
                .GetAssemblies()
                .SelectMany(asm =>
                {
                    try
                    {
                        return asm.GetTypes();
                    }
                    catch (ReflectionTypeLoadException ex)
                    {
                        return ex.Types.Where(t => t != null)!;
                    }
                })
                .Select(type => type!)
                .Where(type => type.GetCustomAttribute<TestSuite>() != null);

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
                    catch (TargetInvocationException tie)
                        when (tie.InnerException is AssertionException)
                    {
                        AssertionException assert = tie.InnerException as AssertionException;
                        DisplayAssertionError(assert);
                        Console.WriteLine("==> Test failed");
                        testSucced = false;
                    }
                    catch (Exception exception)
                    {
                        testSucced = false;
                        Console.WriteLine($"==> Test throw an exception: {exception}");
                    }
                    if (testSucced)
                    {
                        Console.WriteLine($"=== {testCase.Name} | Case Succed");
                    }
                    else
                    {
                        Console.WriteLine($"=== {testCase.Name} | Case Failed");
                    }
                }
            }
        }

        static public void DisplayAssertionError(Exception ex, int depth = 0)
        {
            if (ex is AssertionException)
            {
                string padding = new string('\t', depth);
                AssertionException? assertex = ex as AssertionException;

                Console.Write($"{padding}Assertion failed: ");
                Console.WriteLine(ex.Message);
                Console.WriteLine($"{padding}\tExpected: <{GetNameOrValue(assertex.Expected, depth)}>");
                Console.WriteLine($"{padding}\tActual: <{GetNameOrValue(assertex.Actual, depth)}>");
            }
            if (ex.InnerException != null)
            {
                DisplayAssertionError(ex.InnerException, depth + 1);
            }

            static string GetNameOrValue(object? obj, int depth)
            {
                if (obj == null)
                    return "null";

                Type type = obj.GetType();

                if (type == null)
                    return "Unknown";

                MethodInfo method = type.GetMethod("ToString", Type.EmptyTypes)!;
                if (method.GetBaseDefinition().DeclaringType != typeof(object))
                    return type.FullName!;
                string value = obj.ToString()!;
                if (value.Contains('\n'))
                {
                    string padding = $"\n{new string('\t', depth + 2)}";

                    value = value.Replace("\n", padding);

                    int index = value.LastIndexOf(padding, StringComparison.Ordinal);

                    value = (value.Remove(index, padding.Length) + '\n').Insert(0, padding);
                }
                return value;
            
            }
        }
    }
}
