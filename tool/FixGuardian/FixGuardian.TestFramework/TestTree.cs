using System.Reflection;
using System.Xml;
using FixGuardian.TestFramework.Attributes;

namespace FixGuardian.TestFramework
{
    public class TestTree
    {
        private List<TestNode> FirstLevelTest { get; } = new List<TestNode>();

        public TestTree(IEnumerable<TestGroup> testGroups, TestConfiguration config)
        {
            foreach (TestGroup testGroup in testGroups)
                FirstLevelTest.Add(new TestNode(testGroup));

            foreach (TestNode node in FirstLevelTest)
            {
                if (config.RunRequired)
                {
                    IEnumerable<RequireSuite> requires = node.Group.TestSuiteType.GetCustomAttributes<RequireSuite>();

                    foreach (RequireSuite requireSuite in requires)
                    {
                        TestNode? selectedNode = FirstLevelTest.Where(testNode => testNode.Group.TestSuiteName == requireSuite.SuiteName).FirstOrDefault();

                        if (selectedNode != null)
                            node.AddRequireSuite(selectedNode);
                        else
                            throw new Exception($"Unable to find required test suite: '{requireSuite.SuiteName}' for test suite: '{node.Group.TestSuiteName}'");
                    }
                }
            }
            if (config.SuiteToRun.Count == 0)
                FirstLevelTest = FirstLevelTest.Where(node => node.ReferenceCount == 0).ToList();
            else
                FirstLevelTest = FirstLevelTest.Where(node => config.SuiteToRun.Contains(node.Group.TestSuiteName)).ToList();
        }

        public void Run()
        {
            foreach (TestNode node in FirstLevelTest)
                node.Run();
        }
    }

    public class TestNode
    {
        private List<TestNode> DependOn { get; } = new List<TestNode>();

        public TestGroup Group { get; }

        public uint ReferenceCount { get; private set; }

        private bool AsBeenRun { get; set; } = false;
        private bool RunResult { get; set; } = false;


        public TestNode(TestGroup testGroup)
        {
            Group = testGroup;
        }

        public void AddRequireSuite(TestNode node)
        {
            node.ReferenceCount++;
            DependOn.Add(node);
        }

        public bool Run()
        {
            if (!AsBeenRun)
            {
                AsBeenRun = true;
                foreach (TestNode node in DependOn)
                {
                    if (!node.Run())
                    {
                        Console.WriteLine($"{Group.TestSuiteName}: Cannot processed futher, test node failed: {node.Group.TestSuiteName}");
                        return false;
                    }
                }
                return Group.Run();
            }
            else
            {
                Console.WriteLine($"Test node already run: {Group.TestSuiteName}, with result: {RunResult}");
                return RunResult;
            }
        }
    }
}