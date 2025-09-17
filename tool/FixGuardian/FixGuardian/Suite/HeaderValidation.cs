
using System.Net.Sockets;
using System.Text;
using FixGuardian.Message;
using FixGuardian.Messages.Definition;
using FixGuardian.TestFramework.Assertions;
using FixGuardian.TestFramework.Attributes;
using FixGuardian.Messages.Enums;

namespace FixGuardian.Suite
{
    [TestSuite("Header validation")]
    public class HeaderValidation
    {
        [TestCase("Invalid first positional (BeginString)")]
        public void InvalidBeginString()
        {
            TcpClient tcpClient = new TcpClient("127.0.0.1", 8080);
            NetworkStream stream = tcpClient.GetStream();

            Header header1 = new Header()
            {
                BeginString = null,
                BodyLength = 0,
                MsgType = 'Z',
                SenderCompId = "AZE",
                TargetCompId = "MyMarket",
                MsgSeqNum = 1,
                SendingTime = DateTime.Now
            };

            string headerStr = header1.ToString(FixHelper.NullHandlingStrategy.AllowNull);

            Console.WriteLine($"Sending: '{headerStr.Replace('\u0001', '^')}'");

            byte[] data = Encoding.UTF8.GetBytes(headerStr);
            stream.Write(data, 0, data.Length);
            byte[] buffer = new byte[4096];
            int bytesRead = stream.Read(buffer, 0, buffer.Length);

            string receiveBuffer = Encoding.UTF8.GetString(buffer, 0, bytesRead);
            Console.WriteLine($"Received: '{receiveBuffer.Replace('\u0001', '^')}'");


            var (header, reject) = Assert.Received<SessionReject>(receiveBuffer);
            Assert.Equal(header, new Header()
            {
                BeginString = "FIX.4.2",
                BodyLength = 43,
                MsgType = '3',
                SenderCompId = "MyMarket",
                TargetCompId = "",
                MsgSeqNum = 0,
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

        [TestCase("Invalid third positional (MsgType)")]
        public void InvalidMessageType()
        {
            TcpClient tcpClient = new TcpClient("127.0.0.1", 8080);
            NetworkStream stream = tcpClient.GetStream();

            Header header1 = new Header()
            {
                BeginString = "FIX.4.2",
                BodyLength = 0,
                MsgType = null,
                SenderCompId = "AZE",
                TargetCompId = "MyMarket",
                MsgSeqNum = 1,
                SendingTime = DateTime.Now
            };

            string headerStr = header1.ToString(FixHelper.NullHandlingStrategy.AllowNull);

            Console.WriteLine($"Sending: '{headerStr.Replace('\u0001', '^')}'");

            byte[] data = Encoding.UTF8.GetBytes(headerStr);
            stream.Write(data, 0, data.Length);
            byte[] buffer = new byte[4096];
            int bytesRead = stream.Read(buffer, 0, buffer.Length);

            string receiveBuffer = Encoding.UTF8.GetString(buffer, 0, bytesRead);
            Console.WriteLine($"Received: '{receiveBuffer.Replace('\u0001', '^')}'");


            var (header, reject) = Assert.Received<SessionReject>(receiveBuffer);

            Assert.Equal(header, new Header()
            {
                BeginString = "FIX.4.2",
                BodyLength = 44,
                MsgType = '3',
                SenderCompId = "MyMarket",
                TargetCompId = "",
                MsgSeqNum = 0,
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

            Header header1 = new Header()
            {
                BeginString = "FIX.4.2",
                BodyLength = 0,
                MsgType = 'Z',
                SenderCompId = "AZE",
                TargetCompId = null,
                MsgSeqNum = 0,
                SendingTime = DateTime.Now
            };

            string headerStr = header1.ToString(FixHelper.NullHandlingStrategy.AllowNull);

            Console.WriteLine($"Sending: '{headerStr.Replace('\u0001', '^')}'");

            byte[] data = Encoding.UTF8.GetBytes(headerStr);
            stream.Write(data, 0, data.Length);
            byte[] buffer = new byte[4096];
            int bytesRead = stream.Read(buffer, 0, buffer.Length);

            string receiveBuffer = Encoding.UTF8.GetString(buffer, 0, bytesRead);
            Console.WriteLine($"Received: '{receiveBuffer.Replace('\u0001', '^')}'");


            var (header, reject) = Assert.Received<SessionReject>(receiveBuffer);

            Assert.Equal(header, new Header()
            {
                BeginString = "FIX.4.2",
                BodyLength = 54,
                MsgType = '3',
                SenderCompId = "MyMarket",
                TargetCompId = "",
                MsgSeqNum = 0,
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
            TcpClient tcpClient = new TcpClient("127.0.0.1", 8080);
            NetworkStream stream = tcpClient.GetStream();

            Header header1 = new Header()
            {
                BeginString = "FIX.4.2",
                BodyLength = 0,
                MsgType = 'Z',
                SenderCompId = "AZE",
                TargetCompId = null,
                MsgSeqNum = 0,
                SendingTime = DateTime.Now
            };

            string message = FixHelper.AddCheckSum(header1.ToString(FixHelper.NullHandlingStrategy.AllowNull));
            Console.WriteLine($"Sending: '{message.Replace('\u0001', '^')}'");

            byte[] data = Encoding.UTF8.GetBytes(message);
            stream.Write(data, 0, data.Length);
            byte[] buffer = new byte[4096];
            int bytesRead = stream.Read(buffer, 0, buffer.Length);

            string receiveBuffer = Encoding.UTF8.GetString(buffer, 0, bytesRead);
            Console.WriteLine($"Received: '{receiveBuffer.Replace('\u0001', '^')}'");


            var (header, reject) = Assert.Received<SessionReject>(receiveBuffer);

            Assert.Equal(header, new Header()
            {
                BeginString = "FIX.4.2",
                BodyLength = 63,
                MsgType = '3',
                SenderCompId = "MyMarket",
                TargetCompId = "",
                MsgSeqNum = 0,
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
    }
}