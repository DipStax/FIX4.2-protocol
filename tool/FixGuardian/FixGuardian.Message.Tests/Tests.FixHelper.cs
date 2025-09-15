namespace FixGuardian.Message.Tests
{
    public class FixHelperTest
    {
        [TestFixture]
        public class CheckSum
        {
            [Test]
            public void Arbitrary()
            {
                Assert.That(FixHelper.AddArbitraryCheckSum("", 999), Is.EqualTo("10=999\u0001"));
            }

            [Test]
            public void Calculated()
            {
                string message = "ThisIsAMessage\u0001";

				Assert.That(FixHelper.AddCheckSum(message).Replace('\u0001', '^'), Is.EqualTo("ThisIsAMessage^10=91^"));
			}
		}
	}
}
