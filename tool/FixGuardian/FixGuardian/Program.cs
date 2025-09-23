using System.Reflection;
using FixGuardian.TestFramework.Attributes;
using System;
using System.Runtime.Loader;
using FixGuardian.TestFramework.Assertions;
using System.Security.Cryptography;
using FixGuardian.TestFramework;

namespace FixGuardian
{
    internal class Program
    {
        static void Main(string[] args)
        {
            IEnumerable<TestGroup> testgroups = AppDomain.CurrentDomain
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
                .Where(type => type.GetCustomAttribute<TestSuite>() != null)
                .Select(type => new TestGroup(type));

            TestTree testTree = new TestTree(testgroups, new TestConfiguration()
            {
                RunRequired = false,
                SuiteToRun = new List<string>()
                {
                    // "NewOrderSingle - Conditional invalid",
                    "NewOrderSingle - Single user",
                    "NewOrderSingle - Double user"
                }
            });

            testTree.Run();
        }

    }
}
