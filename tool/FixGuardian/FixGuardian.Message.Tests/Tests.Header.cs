using FixGuardian.Message.Exceptions;

namespace FixGuardian.Message.Tests
{
    public class HeaderTest
    {
        static Header CreateFilledHeader(DateTime time)
        {
            return new Header()
            {
                BeginString = "FIX.4.2",
                MsgSeqNum = 1,
                BodyLength = 0,
                MsgType = 'A',
                SenderCompId = "Sender",
                TargetCompId = "Receiver",
                SendingTime = time
            };
        }

        [Test]
        public void Clear()
        {
            DateTime time = DateTime.Now;
            Header header = CreateFilledHeader(time);

            header.Clear();
            Assert.That(header.BeginString, Is.Null);
            Assert.That(header.MsgSeqNum, Is.Null);
            Assert.That(header.BodyLength, Is.Null);
            Assert.That(header.MsgType, Is.Null);
            Assert.That(header.SenderCompId, Is.Null);
            Assert.That(header.TargetCompId, Is.Null);
            Assert.That(header.SendingTime, Is.Null);
        }

        [TestFixture]
        public class Function_Equals
        {
            [Test]
            public void Equal()
            {
                DateTime time = DateTime.Now;
                Header header1 = CreateFilledHeader(time);
                Header header2 = CreateFilledHeader(time);

                Assert.That(header1.Equals(header2), Is.True);
            }

            [Test]
            public void Equal_BothNull()
            {
                DateTime time = DateTime.Now;
                Header header1 = CreateFilledHeader(time);
                Header header2 = CreateFilledHeader(time);

                header1.BodyLength = null;
                header2.BodyLength = null;

                Assert.That(header1.Equals(header2), Is.True);
            }

            [Test]
            public void NotEqual_Value()
            {
                DateTime time = DateTime.Now;
                Header header1 = CreateFilledHeader(time);
                Header header2 = CreateFilledHeader(time);

                header2.SenderCompId = "OtherSender";

                Assert.That(header1.Equals(header2), Is.False);
            }

            [Test]
            public void NotEqual_Null()
            {
                DateTime time = DateTime.Now;
                Header header1 = CreateFilledHeader(time);
                Header header2 = CreateFilledHeader(time);

                header2.BeginString = null;

                Assert.That(header1.Equals(header2), Is.False);
            }

            [Test]
            public void NotEqual_DateTime()
            {
                DateTime time = DateTime.Now;
                Header header1 = CreateFilledHeader(time);
                Header header2 = CreateFilledHeader(time);

                header2.SendingTime = new DateTime(1950, 05, 15, 20, 49, 27);

                Assert.That(header1.Equals(header2), Is.False);
            }
        }


        [TestFixture]
        public class Function_ToString
        {
            public Header TestHeader { get; set; } = new Header();
            
            [SetUp]
            public void SetUp()
            {
                DateTime time = new DateTime(2025, 09, 25, 12, 25, 30);

                TestHeader = CreateFilledHeader(time);
            }

            [Test]
            public void Default_Valid()
            {
                const string expected = "8=FIX.4.2^9=0^35=A^49=Sender^56=Receiver^34=1^52=20250925-12:25:30^";

                Assert.That(
                    TestHeader.ToString().Replace('\u0001', '^'),
                    Is.EqualTo(expected)
                );
            }

            [Test]
            public void Default_NullPositional()
            {
                TestHeader.BeginString = null;

                FixEncodeException? fixEncodeException = Assert.Throws<FixEncodeException>(() => TestHeader.ToString());
                Assert.That(fixEncodeException, Is.Not.Null);
                Assert.That(fixEncodeException!.RefTagId, Is.EqualTo(8));
            }

            [Test]
            public void Default_NullNonPosition()
            {
                TestHeader.SenderCompId = null;

                FixEncodeException? fixEncodeException = Assert.Throws<FixEncodeException>(() => TestHeader.ToString());
                Assert.That(fixEncodeException, Is.Not.Null);
                Assert.That(fixEncodeException!.RefTagId, Is.EqualTo(49));
            }

            // todo add ToString_Default_NullOptional: test a null value on an optional

            [Test]
            public void AllowNull_NullPositional()
            {
                const string expected = "9=0^35=A^49=Sender^56=Receiver^34=1^52=20250925-12:25:30^";

                TestHeader.BeginString = null;

                Assert.That(
                    TestHeader.ToString(Header.ToStringContext.AllowNull).Replace('\u0001', '^'),
                    Is.EqualTo(expected)
                );
            }

            [Test]
            public void AllowNull_NullNonPositional()
            {
                const string expected = "8=FIX.4.2^9=0^35=A^49=Sender^34=1^52=20250925-12:25:30^";

                TestHeader.TargetCompId = null;

                Assert.That(
                    TestHeader.ToString(Header.ToStringContext.AllowNull).Replace('\u0001', '^'),
                    Is.EqualTo(expected)
                );
            }

            [Test]
            public void NullAsEmpty_NullPositional()
            {
                const string expected = "8=FIX.4.2^9=^35=A^49=Sender^56=Receiver^34=1^52=20250925-12:25:30^";

                TestHeader.BodyLength = null;

                Assert.That(
                    TestHeader.ToString(Header.ToStringContext.NullAsEmpty).Replace('\u0001', '^'),
                    Is.EqualTo(expected)
                );
            }

            [Test]
            public void NullAsEmpty_NullNonPositional()
            {
                const string expected = "8=FIX.4.2^9=0^35=A^49=^56=Receiver^34=1^52=20250925-12:25:30^";

                TestHeader.SenderCompId = null;

                Assert.That(
                    TestHeader.ToString(Header.ToStringContext.NullAsEmpty).Replace('\u0001', '^'),
                    Is.EqualTo(expected)
                );
            }

            [Test]
            public void NullAsEmptyTag_NullPositional()
            {
                const string expected = "8=FIX.4.2^9=0^^49=Sender^56=Receiver^34=1^52=20250925-12:25:30^";

                TestHeader.MsgType = null;

                Assert.That(
                    TestHeader.ToString(Header.ToStringContext.NullAsEmptyTag).Replace('\u0001', '^'),
                    Is.EqualTo(expected)
                );
            }

            [Test]
            public void NullAsEmptyTag_NullNonPositional()
            {
                const string expected = "8=FIX.4.2^9=0^35=A^49=Sender^56=Receiver^^52=20250925-12:25:30^";

                TestHeader.MsgSeqNum = null;

                Assert.That(
                    TestHeader.ToString(Header.ToStringContext.NullAsEmptyTag).Replace('\u0001', '^'),
                    Is.EqualTo(expected)
                );
            }
        }

        [TestFixture]
        public class Function_FromString_InOrder
        {
            static public DateTime TimeNow = DateTime.Now;

            public List<KeyValuePair<ushort, string>> MapHeader { get; set; }
            public Header TestHeader { get; set; } = new Header();
            public Header ExpectedHeader { get; set; }

            [SetUp]
            public void SetUp()
            {
                ExpectedHeader = new Header()
                {
                    BeginString = "FIX.4.2",
                    BodyLength = 0,
                    MsgType = 'C',
                    SenderCompId = "Sender",
                    TargetCompId = "Target",
                    MsgSeqNum = 1,
                    SendingTime = TimeNow
                };
                MapHeader = new List<KeyValuePair<ushort, string>>()
                {
                    new KeyValuePair<ushort, string>(8, "FIX.4.2"),
                    new KeyValuePair<ushort, string>(9, "0"),
                    new KeyValuePair<ushort, string>(35, "C"),
                    new KeyValuePair<ushort, string>(49, "Sender"),
                    new KeyValuePair<ushort, string>(56, "Target"),
                    new KeyValuePair<ushort, string>(34, "1"),
                    new KeyValuePair<ushort, string>(52, TimeNow.ToString("yyyyMMdd-HH:mm:ss")),
                };
            }

            [Test]
            public void EmptyMessage()
            {
                Header testHeader = new Header();

                FixDecodeException? fixDecodeException = Assert.Throws<FixDecodeException>(() => testHeader.FromString(new List<KeyValuePair<ushort, string>>()));

                Assert.That(fixDecodeException, Is.Not.Null);
            }

            [Test]
            public void ValidMessage()
            {
                TestHeader.FromString(MapHeader);

                Assert.That(TestHeader, Is.EqualTo(ExpectedHeader));
                Assert.That(MapHeader, Is.Empty);
            }

            [Test]
            public void Positional_MissingFirst()
            {
                List<KeyValuePair<ushort, string>> expected = new List<KeyValuePair<ushort, string>>()
                {
                    new KeyValuePair<ushort, string>(9, "0"),
                    new KeyValuePair<ushort, string>(35, "C"),
                    new KeyValuePair<ushort, string>(49, "Sender"),
                    new KeyValuePair<ushort, string>(56, "Target"),
                    new KeyValuePair<ushort, string>(34, "1"),
                    new KeyValuePair<ushort, string>(52, TimeNow.ToString("yyyyMMdd-HH:mm:ss"))
                };
                MapHeader.RemoveAt(0);

                FixDecodeException? fixDecodeException = Assert.Throws<FixDecodeException>(() => TestHeader.FromString(MapHeader));
                Assert.That(fixDecodeException, Is.Not.Null);
                Assert.That(MapHeader, Is.EqualTo(expected));
            }

            [Test]
            public void Positional_MissingSecond()
            {
                List<KeyValuePair<ushort, string>> expected = new List<KeyValuePair<ushort, string>>()
                {
                    new KeyValuePair<ushort, string>(35, "C"),
                    new KeyValuePair<ushort, string>(49, "Sender"),
                    new KeyValuePair<ushort, string>(56, "Target"),
                    new KeyValuePair<ushort, string>(34, "1"),
                    new KeyValuePair<ushort, string>(52, TimeNow.ToString("yyyyMMdd-HH:mm:ss"))
                };
                MapHeader.RemoveAt(1);

                FixDecodeException? fixDecodeException = Assert.Throws<FixDecodeException>(() => TestHeader.FromString(MapHeader));
                Assert.That(fixDecodeException, Is.Not.Null);
                Assert.That(MapHeader, Is.EqualTo(expected));
            }

            [Test]
            public void NonPositional_MissingAny()
            {
                MapHeader.RemoveAt(4);

                FixDecodeException? fixDecodeException = Assert.Throws<FixDecodeException>(() => TestHeader.FromString(MapHeader));
                Assert.That(fixDecodeException, Is.Not.Null);
                Assert.That(MapHeader, Is.Empty);
            }

            [Test]
            public void RemainOneItem()
            {
                KeyValuePair<ushort, string> newValue = new KeyValuePair<ushort, string>(999, "value");
                List<KeyValuePair<ushort, string>> expected = new List<KeyValuePair<ushort, string>>()
                {
                    newValue
                };
                MapHeader.Add(newValue);

                TestHeader.FromString(MapHeader);
                Assert.That(MapHeader, Is.EqualTo(expected));
            }

            [Test]
            public void DuplicateNonPositional()
            {
                KeyValuePair<ushort, string> newValue = new KeyValuePair<ushort, string>(49, "Sender");
                List<KeyValuePair<ushort, string>> expected = new List<KeyValuePair<ushort, string>>()
                {
                    newValue
                };
                MapHeader.Add(newValue);

                FixDecodeException? fixDecodeException = Assert.Throws<FixDecodeException>(() => TestHeader.FromString(MapHeader));
                Assert.That(fixDecodeException, Is.Not.Null);
                Assert.That(MapHeader, Is.EqualTo(expected));
            }
        }
    }
}