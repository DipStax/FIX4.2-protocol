using System.Reflection;
using System.Security.Cryptography;
using FixGuardian.TestFramework.Assertions;
using FixGuardian.TestFramework.Attributes;

namespace FixGuardian.TestFramework
{
    public class TestGroup
    {
        public Type TestSuiteType { get; }
        public string TestSuiteName { get; }

        private MethodInfo? Setup { get; set; }
        private MethodInfo? TearDown { get; set; }

        private IEnumerable<MethodInfo> TestCases { get; }

        public TestGroup(Type type)
        {
            TestSuiteType = type;

            TestSuite? suite = type.GetCustomAttribute<TestSuite>();

            if (suite == null)
                throw new Exception($"Invalid test suite type: <{TestSuiteType.Name}>");
            TestSuiteName = suite.Name;

            MethodInfo[] methods = TestSuiteType.GetMethods();
            IEnumerable<MethodInfo> setup = methods.Where(method => method.GetCustomAttribute<TestSetup>() != null);
            IEnumerable<MethodInfo> teardown = methods.Where(method => method.GetCustomAttribute<TestTearDown>() != null);

            if (setup.Count() > 1)
                throw new Exception($"Only one setup is allowed, found {setup.Count()} in {type.FullName}");
            Setup = setup.FirstOrDefault();
            if (teardown.Count() > 1)
                throw new Exception($"Only one teardown is allowed, found {teardown.Count()} in {type.FullName}");
            TearDown = teardown.FirstOrDefault();
            TestCases = methods.Where(method => method.GetCustomAttribute<TestCase>() != null);
        }

        public void Run()
        {
            object? testSuiteInstance = Activator.CreateInstance(TestSuiteType);

            if (testSuiteInstance == null)
                throw new Exception($"Failed to instiate the type: <{TestSuiteType.FullName}>");
            Console.WriteLine($"=== Running Test Suite: <{TestSuiteType.FullName}> {TestSuiteName}");
            foreach (MethodInfo method in TestCases)
            {
                bool testSucced = true;
                TestCase testCase = method.GetCustomAttribute<TestCase>()!;

                Console.WriteLine($"=== Running test case: <{TestSuiteType.FullName}> {testCase.Name}");
                try
                {
                    if (Setup != null)
                        Setup.Invoke(testSuiteInstance, null);
                    method.Invoke(testSuiteInstance, null);
                    if (TearDown != null)
                        TearDown.Invoke(testSuiteInstance, null);
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
                    Console.WriteLine($"=== <{TestSuiteType.FullName}> {testCase.Name} | Case Succed");
                }
                else
                {
                    Console.WriteLine($"=== <{TestSuiteType.FullName}> {testCase.Name} | Case Failed");
                }
            }
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