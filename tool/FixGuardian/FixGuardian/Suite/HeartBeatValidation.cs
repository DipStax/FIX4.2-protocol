using FixGuardian.Messages.Definition;
using FixGuardian.TestFramework;
using FixGuardian.TestFramework.Assertions;
using FixGuardian.TestFramework.Attributes;

namespace FixGuardian.Suite
{
    [RequireSuite("Logout validation")]
    [TestSuite("HeartBeat validation")]
    public class HeartBeatValidation
    {
        public FixClient Client;

        [TestSetup()]
        public void Setup()
        {
            Client = new FixClient("Sender");

            Client.Logon(1);
        }

        public void TearDown()
        {
            if (Client.IsConnected())
                Client.Logout();
        }

        [TestCase("Valid HeartBeat")]
        public void Valid()
        {
            Client.Send(new HeartBeat());
            Client.Receive<HeartBeat>();
        }

        [TestCase("Fail HeartBeat")]
        public void FailHeartBeat()
        {
            TestRequest testreq = Client.Receive<TestRequest>();
            Assert.NotNull(testreq.TestReqId);

            Client.Send(new HeartBeat()
            {
                TestReqId = testreq.TestReqId
            });
            Client.Receive<HeartBeat>();
        }

        [TestCase("Fail HeartBeat and TestRequest")]
        public void FailBoth()
        {
            TestRequest hb = Client.Receive<TestRequest>();
            Assert.NotNull(hb.TestReqId);

            Thread.Sleep(2000);
            Client.AssertServerDisconnet();
        }
    }
}