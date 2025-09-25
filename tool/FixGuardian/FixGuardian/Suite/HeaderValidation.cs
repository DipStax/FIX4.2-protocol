
using System.Net.Sockets;
using System.Text;
using FixGuardian.Messages;
using FixGuardian.Messages.Definition;
using FixGuardian.TestFramework.Assertions;
using FixGuardian.TestFramework.Attributes;
using FixGuardian.Messages.Enums;

namespace FixGuardian.Suite
{
    [TestSuite("Header validation")]
    public class HeaderValidation
    {
        public TcpClient TcpSocket;
        public NetworkStream TcpStream;

        [TestSetup]
        public void Setup()
        {
            TcpSocket = new TcpClient("127.0.0.1", 8080);
            TcpStream = TcpSocket.GetStream();
        }

        [TestTearDown]
        public void TearDown()
        {
            TcpStream.Close();
            TcpSocket.Close();
        }

        private void Send(string msg)
        {
            byte[] data = Encoding.UTF8.GetBytes(msg);
            TcpStream.Write(data, 0, data.Length);
        }

        private string Receive()
        {
            byte[] buffer = new byte[4096];
            int bytesRead = TcpStream.Read(buffer, 0, buffer.Length);

            return Encoding.UTF8.GetString(buffer, 0, bytesRead);
        }

        [TestCase("Invalid first positional (BeginString)")]
        public void InvalidBeginString()
        {
            Header headerSend = new Header()
            {
                BeginString = null,
                BodyLength = 0,
                MsgType = 'Z',
                SenderCompId = "Sender",
                TargetCompId = "MyMarket",
                MsgSeqNum = 0,
                SendingTime = DateTime.Now
            };
            Send(headerSend.ToString(FixHelper.NullHandlingStrategy.AllowNull));

            var (header, reject) = Assert.Received<SessionReject>(Receive());
            Assert.Equal(header, new Header()
            {
                BeginString = "FIX.4.2",
                BodyLength = 43,
                MsgType = '3',
                SenderCompId = "MyMarket",
                TargetCompId = "",
                MsgSeqNum = 1,
                SendingTime = header.SendingTime,
            });
            Assert.Equal(reject, new SessionReject()
            {
                RefMsgType = null,
                RefSeqNum = 0,
                RefTagId = 8,
                SessionRejectReason = RejectReasonSession.InvalidTagNum,
                Text = "Invalid positional tag"
            });
        }

        [TestCase("Invalid first positional (BeginString) Fully Empty")]
        public void InvalidBeginStringFullEmpty()
        {
            Header headerSend = new Header()
            {
                BeginString = null,
                BodyLength = 0,
                MsgType = 'Z',
                SenderCompId = "Sender",
                TargetCompId = "MyMarket",
                MsgSeqNum = 0,
                SendingTime = DateTime.Now
            };
            Send(headerSend.ToString(FixHelper.NullHandlingStrategy.NullAsEmpty));

            var (header, reject) = Assert.Received<SessionReject>(Receive());
            Assert.Equal(header, new Header()
            {
                BeginString = "FIX.4.2",
                BodyLength = 47,
                MsgType = '3',
                SenderCompId = "MyMarket",
                TargetCompId = "",
                MsgSeqNum = 1,
                SendingTime = header.SendingTime,
            });
            Assert.Equal(reject, new SessionReject()
            {
                RefMsgType = null,
                RefSeqNum = 0,
                RefTagId = null,
                SessionRejectReason = RejectReasonSession.UndefineTag,
                Text = "Unable to parse header key-value"
            });
        }

        [TestCase("Invalid first positional (BeginString) Null As Fully Empty")]
        public void InvalidBeginStringNullAsFullyEmpty()
        {
            Header headerSend = new Header()
            {
                BeginString = null,
                BodyLength = 0,
                MsgType = 'Z',
                SenderCompId = "Sender",
                TargetCompId = "MyMarket",
                MsgSeqNum = 0,
                SendingTime = DateTime.Now
            };
            Send(headerSend.ToString(FixHelper.NullHandlingStrategy.NullAsFullyEmpty));

            var (header, reject) = Assert.Received<SessionReject>(Receive());
            Assert.Equal(header, new Header()
            {
                BeginString = "FIX.4.2",
                BodyLength = 47,
                MsgType = '3',
                SenderCompId = "MyMarket",
                TargetCompId = "",
                MsgSeqNum = 1,
                SendingTime = header.SendingTime,
            });
            Assert.Equal(reject, new SessionReject()
            {
                RefMsgType = null,
                RefSeqNum = 0,
                RefTagId = null,
                SessionRejectReason = RejectReasonSession.UndefineTag,
                Text = "Unable to parse header key-value"
            });
        }

        [TestCase("Invalid third positional (MsgType)")]
        public void InvalidMessageType()
        {
            Header headerSend = new Header()
            {
                BeginString = "FIX.4.2",
                BodyLength = 0,
                MsgType = null,
                SenderCompId = "Sender",
                TargetCompId = "MyMarket",
                MsgSeqNum = 0,
                SendingTime = DateTime.Now
            };
            Send(headerSend.ToString(FixHelper.NullHandlingStrategy.AllowNull));

            var (header, reject) = Assert.Received<SessionReject>(Receive());
            Assert.Equal(header, new Header()
            {
                BeginString = "FIX.4.2",
                BodyLength = 44,
                MsgType = '3',
                SenderCompId = "MyMarket",
                TargetCompId = "",
                MsgSeqNum = 1,
                SendingTime = header.SendingTime,
            });
            Assert.Equal(reject, new SessionReject()
            {
                RefMsgType = null,
                RefSeqNum = 0,
                RefTagId = 35,
                SessionRejectReason = RejectReasonSession.InvalidTagNum,
                Text = "Invalid positional tag"
            });
        }

        [TestCase("Invalid non positional without checksum")]
        public void InvalidTargetCompIdWithoutCheckSum()
        {
            TcpClient tcpClient = new TcpClient("127.0.0.1", 8080);
            NetworkStream stream = tcpClient.GetStream();

            Header headerSend = new Header()
            {
                BeginString = "FIX.4.2",
                BodyLength = 0,
                MsgType = 'Z',
                SenderCompId = "Sender",
                TargetCompId = null,
                MsgSeqNum = 0,
                SendingTime = DateTime.Now
            };
            Send(headerSend.ToString(FixHelper.NullHandlingStrategy.AllowNull));

            var (header, reject) = Assert.Received<SessionReject>(Receive());
            Assert.Equal(header, new Header()
            {
                BeginString = "FIX.4.2",
                BodyLength = 55,
                MsgType = '3',
                SenderCompId = "MyMarket",
                TargetCompId = "",
                MsgSeqNum = 1,
                SendingTime = header.SendingTime,
            });
            Assert.Equal(reject, new SessionReject()
            {
                RefMsgType = 'Z',
                RefSeqNum = 0,
                RefTagId = 10,
                SessionRejectReason = RejectReasonSession.RequiredTagMissing,
                Text = "Last tag should be CheckSum"
            });
        }

        [TestCase("Invalid non positional with checksum")]
        public void InvalidTargetCompIdWithCheckSum()
        {
            Header headerSend = new Header()
            {
                BeginString = "FIX.4.2",
                BodyLength = 0,
                MsgType = 'Z',
                SenderCompId = "Sender",
                TargetCompId = null,
                MsgSeqNum = 0,
                SendingTime = DateTime.Now
            };
            Send(FixHelper.AddCheckSum(headerSend.ToString(FixHelper.NullHandlingStrategy.AllowNull)));

            var (header, reject) = Assert.Received<SessionReject>(Receive());
            Assert.Equal(header, new Header()
            {
                BeginString = "FIX.4.2",
                BodyLength = 63,
                MsgType = '3',
                SenderCompId = "MyMarket",
                TargetCompId = "",
                MsgSeqNum = 1,
                SendingTime = header.SendingTime,
            });
            Assert.Equal(reject, new SessionReject()
            {
                RefMsgType = 'Z',
                RefSeqNum = 0,
                RefTagId = 56,
                SessionRejectReason = RejectReasonSession.RequiredTagMissing,
                Text = "Non positional required tag missing"
            });
        }

        [TestCase("Valid Header - Wrong BeginString")]
        public void ValidHeaderWrongBeginString()
        {
            Header headerSend = new Header()
            {
                BeginString = "WrongBeginString",
                BodyLength = 0,
                MsgType = 'Z',
                SenderCompId = "Sender",
                TargetCompId = "MyMarket",
                MsgSeqNum = 0,
                SendingTime = DateTime.Now
            };
            Send(FixHelper.AddCheckSum(headerSend.ToString(FixHelper.NullHandlingStrategy.AllowNull)));

            var (header, reject) = Assert.Received<SessionReject>(Receive());
            Assert.Equal(header, new Header()
            {
                BeginString = "FIX.4.2",
                BodyLength = 67,
                MsgType = '3',
                SenderCompId = "MyMarket",
                TargetCompId = "",
                MsgSeqNum = 1,
                SendingTime = header.SendingTime,
            });
            Assert.Equal(reject, new SessionReject()
            {
                RefMsgType = 'Z',
                RefSeqNum = 0,
                RefTagId = 8,
                SessionRejectReason = RejectReasonSession.ValueOutOfRange,
                Text = "Incorrect begin string. Expected FIX.4.2"
            });
        }

        [TestCase("Valid Header - Invalid MsgType")]
        public void ValidHeaderInvalidMsgType()
        {
            Header headerSend = new Header()
            {
                BeginString = "FIX.4.2",
                BodyLength = 0,
                MsgType = 'Z',
                SenderCompId = "Sender",
                TargetCompId = "MyMarket",
                MsgSeqNum = 0,
                SendingTime = DateTime.Now
            };
            Send(FixHelper.AddCheckSum(headerSend.ToString(FixHelper.NullHandlingStrategy.AllowNull)));

            var (header, reject) = Assert.Received<SessionReject>(Receive());
            Assert.Equal(header, new Header()
            {
                BeginString = "FIX.4.2",
                BodyLength = 55,
                MsgType = '3',
                SenderCompId = "MyMarket",
                TargetCompId = "",
                MsgSeqNum = 1,
                SendingTime = header.SendingTime,
            });
            Assert.Equal(reject, new SessionReject()
            {
                RefMsgType = 'Z',
                RefSeqNum = 0,
                RefTagId = 35,
                SessionRejectReason = RejectReasonSession.InvalidMsgType,
                Text = "Not supported message type"
            });
        }
    }
}