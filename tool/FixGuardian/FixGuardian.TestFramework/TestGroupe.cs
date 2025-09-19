using System.Linq;
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

        private IEnumerable<TestMethod> TestCase { get; }

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
            TestCase = methods
                .Where(method => method.GetCustomAttribute<TestCase>() != null)
                .Select(method => new TestMethod(method, Setup, TearDown));
        }

        public void Run()
        {
            object? testSuiteInstance = Activator.CreateInstance(TestSuiteType);

            if (testSuiteInstance == null)
                throw new Exception($"Failed to instiate the type: <{TestSuiteType.FullName}>");
            Console.WriteLine($"=== Running Test Suite: <{TestSuiteType.FullName}> {TestSuiteName}");
            foreach (TestMethod method in TestCase)
            {
                method.Run(testSuiteInstance);
            }
        }
    }
}