namespace FixGuardian.TestFramework
{
    public class TestConfiguration
    {
        public bool RunRequired { get; set; } = true;
        public List<string> SuiteToRun { get; set; } = new List<string>();
    };
}