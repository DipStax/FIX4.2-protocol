using System.Reflection;
using FixGuardian.TestFramework.Assertions;
using FixGuardian.TestFramework.Attributes;

namespace FixGuardian.TestFramework
{
    public class TestMethod
    {
        private MethodInfo Method { get; }
        private MethodInfo? Setup { get; } = null;
        private MethodInfo? TearDown { get; } = null;

        public TestMethod(MethodInfo method, MethodInfo? setup, MethodInfo? teardown)
        {
            Method = method;
            Setup = setup;
            TearDown = teardown;
        }

        public bool Run(object testSuiteInstance)
        {
            IEnumerable<TestInput> inputs = Method.GetCustomAttributes<TestInput>();
            bool result = true;


            if (inputs.Count() > 0)
                foreach (TestInput input in inputs)
                    result &= RunTest(testSuiteInstance, Method, input.Data);
            else
                result = RunTest(testSuiteInstance, Method, null);
            return result;
        }

        private bool RunTest(object testSuiteInstance, MethodInfo method, params object?[]? param)
        {
            TestCase testCase = method.GetCustomAttribute<TestCase>()!;

            bool testSucced = true;
            Console.WriteLine($"\t=== Running test case: {testCase.Name}");
            try
            {
                if (Setup != null)
                {
                    try
                    {
                        Setup.Invoke(testSuiteInstance, null);
                    }
                    catch (TargetInvocationException tie)
                        when (tie.InnerException is AssertionException)
                    {
                        throw new AssertionException("In setup function", tie.InnerException);
                    }
                }
                method.Invoke(testSuiteInstance, param);
                if (TearDown != null)
                {
                    try
                    {
                        TearDown.Invoke(testSuiteInstance, null);
                    }
                    catch (TargetInvocationException tie)
                        when (tie.InnerException is AssertionException)
                    {
                        throw new AssertionException("In teardown function", tie.InnerException);
                    }
                }
            }
            catch (TargetInvocationException tie)
                when (tie.InnerException is AssertionException)
            {
                AssertionException assert = tie.InnerException as AssertionException;
                DisplayAssertionError(assert);
                Console.WriteLine("\t==> Test failed");
                testSucced = false;
            }
            catch (Exception exception)
            {
                testSucced = false;
                Console.WriteLine($"\t==> Test throw an exception: {exception}");
            }
            if (testSucced)
            {
                Console.WriteLine($"\t=== Case Succed | {testCase.Name} ");
            }
            else
            {
                Console.WriteLine($"\t=== Case Failed | {testCase.Name}");
            }
            return testSucced;
        }

        static private void DisplayAssertionError(Exception ex, int depth = 0)
        {
            string padding = new string('\t', depth);

            if (ex is AssertionException)
            {
                AssertionException? assertex = ex as AssertionException;

                Console.Write($"{padding}Assertion failed: ");
                Console.WriteLine(ex.Message);
                if (ex.InnerException != null)
                {
                    DisplayAssertionError(ex.InnerException, depth + 1);
                }
                else
                {
                    Console.WriteLine($"{padding}\tExpected: <{GetNameOrValue(assertex.Expected, depth)}>");
                    Console.WriteLine($"{padding}\tActual: <{GetNameOrValue(assertex.Actual, depth)}>");
                }
            }
            else
            {
                Console.WriteLine($"{padding}Error message: {ex.Message}");
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