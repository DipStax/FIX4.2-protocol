using FixGuardian.Messages.Exceptions;

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

        [TestFixture]
        public class Function_ToMap
        {
            [Test]
            public void EmptyMessage()
            {
                Assert.That(FixHelper.ToMap(""), Is.Empty);
            }

            [Test]
            public void WithTrailing()
            {
                FixDecodeException? exception = Assert.Throws<FixDecodeException>(() => FixHelper.ToMap("key=value\u0001trailing"));
                Assert.That(exception, Is.Not.Null);
            }

            [Test]
            public void EmptyField()
            {
                FixDecodeException? exception = Assert.Throws<FixDecodeException>(() => FixHelper.ToMap("key=value\u0001\u0001"));
                Assert.That(exception, Is.Not.Null);
            }

            [Test]
            public void FailTagConvertion()
            {
                FormatException? exception = Assert.Throws<FormatException>(() => FixHelper.ToMap("tag=1\u0001"));
                Assert.That(exception, Is.Not.Null);
            }

            [Test]
            public void MultipleEqual()
            {
                FixDecodeException? exception = Assert.Throws<FixDecodeException>(() => FixHelper.ToMap("1==1\u0001"));
                Assert.That(exception, Is.Not.Null);
            }

            [Test]
            public void Valid()
            {
                const string message = "1=value\u00012=123\u0001";
                List<KeyValuePair<ushort, string>> expected = new List<KeyValuePair<ushort, string>>()
                {
                    new KeyValuePair<ushort, string>(1, "value"),
                    new KeyValuePair<ushort, string>(2, "123")
                };

                Assert.That(FixHelper.ToMap(message), Is.EqualTo(expected));
            }
        }

        [TestFixture]
        public class Function_ToString
        {
            public TestMessage MessageTest { get; set; }

            [SetUp]
            public void SetUp()
            {
                MessageTest = new TestMessage()
                {
                    StringTag = "Test",
                    UshortTag = 123,
                    EnumTag = TestMessage.TestEnum.Value2,
                    OptionalStringTag = "Test2",
                    OptionalUShortTag = 456,
                    OptionalEnumTag = TestMessage.TestEnum.Value1
                };
            }

            [Test]
            public void AllowNull_NonOptional()
            {
                const string expected = "0=Test^1=123^3=Test2^4=456^5=0^";

                MessageTest.EnumTag = null;

                Assert.That(
                    FixHelper.ToString(MessageTest, FixHelper.NullHandlingStrategy.AllowNull).Replace('\u0001', '^'),
                    Is.EqualTo(expected)
                );
            }

            [Test]
            public void AllowNull_Optional()
            {
                const string expected = "0=Test^1=123^2=A^3=Test2^5=0^";

                MessageTest.OptionalUShortTag = null;

                Assert.That(
                    FixHelper.ToString(MessageTest, FixHelper.NullHandlingStrategy.AllowNull).Replace('\u0001', '^'),
                    Is.EqualTo(expected)
                );
            }

            [Test]
            public void Default_NonOptional()
            {
                MessageTest.StringTag = null;

                FixEncodeException? fixDecodeException = Assert.Throws<FixEncodeException>(() => FixHelper.ToString(MessageTest));
                Assert.That(fixDecodeException, Is.Not.Null);
            }

            [Test]
            public void Default_Optional()
            {
                const string expected = "0=Test^1=123^2=A^3=Test2^5=0^";

                MessageTest.OptionalUShortTag = null;

                Assert.That(
                    FixHelper.ToString(MessageTest, FixHelper.NullHandlingStrategy.AllowNull).Replace('\u0001', '^'),
                    Is.EqualTo(expected)
                );
            }

            [Test]
            public void Default_Valid()
            {
                const string expected = "0=Test^1=123^2=A^3=Test2^4=456^5=0^";

                Assert.That(
                    FixHelper.ToString(MessageTest, FixHelper.NullHandlingStrategy.AllowNull).Replace('\u0001', '^'),
                    Is.EqualTo(expected)
                );
            }

            [Test]
            public void NullAsFulyEmpty_NonOptional()
            {
                const string expected = "0=Test^2=A^3=Test2^4=456^5=0^";

                MessageTest.UshortTag = null;

                Assert.That(
                    FixHelper.ToString(MessageTest, FixHelper.NullHandlingStrategy.AllowNull).Replace('\u0001', '^'),
                    Is.EqualTo(expected)
                );
            }

            [Test]
            public void NullAsFulyEmpty_Optional()
            {
                const string expected = "0=Test^1=123^2=A^4=456^5=0^";

                MessageTest.OptionalStringTag = null;

                Assert.That(
                    FixHelper.ToString(MessageTest, FixHelper.NullHandlingStrategy.AllowNull).Replace('\u0001', '^'),
                    Is.EqualTo(expected)
                );
            }
        }

        [TestFixture]
        public class Function_FromString
        {
            [Test]
            public void Valid_InOrder_NoMissing()
            {
                List<KeyValuePair<ushort, string>> mapmsg = new List<KeyValuePair<ushort, string>>()
                {
                    new KeyValuePair<ushort, string>(0, "Test1"),
                    new KeyValuePair<ushort, string>(1, "123"),
                    new KeyValuePair<ushort, string>(2, "A"),
                    new KeyValuePair<ushort, string>(3, "Test2"),
                    new KeyValuePair<ushort, string>(4, "456"),
                    new KeyValuePair<ushort, string>(5, "0"),
                };
                TestMessage expected = new TestMessage()
                {
                    StringTag = "Test1",
                    UshortTag = 123,
                    EnumTag = TestMessage.TestEnum.Value2,
                    OptionalStringTag = "Test2",
                    OptionalUShortTag = 456,
                    OptionalEnumTag = TestMessage.TestEnum.Value1
                };

                TestMessage result = FixHelper.FromString<TestMessage>(mapmsg);

                Assert.That(result, Is.EqualTo(expected));
            }

            [Test]
            public void Valid_InOrder_RequireMissing()
            {
                List<KeyValuePair<ushort, string>> mapmsg = new List<KeyValuePair<ushort, string>>()
                {
                    new KeyValuePair<ushort, string>(0, "Test1"),
                    new KeyValuePair<ushort, string>(2, "A"),
                    new KeyValuePair<ushort, string>(3, "Test2"),
                    new KeyValuePair<ushort, string>(4, "456"),
                    new KeyValuePair<ushort, string>(5, "0"),
                };
                
                FixDecodeException? fixDecodeException = Assert.Throws<FixDecodeException>(() => FixHelper.FromString<TestMessage>(mapmsg));
                Assert.That(fixDecodeException, Is.Not.Null);
            }

            [Test]
            public void Valid_InOrder_Optional()
            {
                List<KeyValuePair<ushort, string>> mapmsg = new List<KeyValuePair<ushort, string>>()
                {
                    new KeyValuePair<ushort, string>(0, "Test1"),
                    new KeyValuePair<ushort, string>(1, "123"),
                    new KeyValuePair<ushort, string>(2, "A"),
                    new KeyValuePair<ushort, string>(3, "Test2"),
                    new KeyValuePair<ushort, string>(5, "0"),
                };
                TestMessage expected = new TestMessage()
                {
                    StringTag = "Test1",
                    UshortTag = 123,
                    EnumTag = TestMessage.TestEnum.Value2,
                    OptionalStringTag = "Test2",
                    OptionalUShortTag = null,
                    OptionalEnumTag = TestMessage.TestEnum.Value1
                };

                TestMessage result = FixHelper.FromString<TestMessage>(mapmsg);

                Assert.That(result, Is.EqualTo(expected));
            }
        }
	}
}
