using FixGuardian.Message;
using FixGuardian.Messages.Definition;
using FixGuardian.TestFramework.Assertions;
using FixGuardian.TestFramework.Attributes;
using FixGuardian.Messages.Enums;
using System.Net.Sockets;
using System.Text;

namespace FixGuardian.Suite
{
    // [RequiredSuite("Header Validation")]
    [TestSuite("Logon validation")]
    public class LogonValidation
    {
        public TcpClient TcpSocket;
        public NetworkStream TcpStream;

        [TestSetup()]
        public void Setup()
        {
            TcpSocket = new TcpClient("127.0.0.1", 8080);
            TcpStream = TcpSocket.GetStream();
        }

        [TestTearDown()]
        public void TearDown()
        {
            TcpStream.Close();
            TcpSocket.Close();
        }

        private void Send(Logon logon, uint seqnum = 0)
        {
            string body = FixHelper.ToString(logon);
            Header header = new Header()
            {
                BeginString = "FIX.4.2",
                BodyLength = (uint)body.Length,
                MsgType = 'A',
                SenderCompId = "Sender",
                TargetCompId = "MyMarket",
                MsgSeqNum = seqnum,
                SendingTime = DateTime.Now
            };
            byte[] data = Encoding.UTF8.GetBytes(FixHelper.AddCheckSum(header.ToString() + body));
            TcpStream.Write(data, 0, data.Length);
        }

        private string Receive()
        {
            byte[] buffer = new byte[4096];
            int bytesRead = TcpStream.Read(buffer, 0, buffer.Length);

            return Encoding.UTF8.GetString(buffer, 0, bytesRead);
        }

        [TestCase("Valid Logon")]
        public void ValidLogon()
        {
            Logon logonSend = new Logon()
            {
                EncryptMethod = EncryptionMethod.None,
                HeartBtInt = 10,
            };
            Send(logonSend);

            var (header, logon) = Assert.Received<Logon>(Receive());
            Assert.Equal(header, new Header()
            {
                BeginString = "FIX.4.2",
                BodyLength = 12,
                MsgType = 'A',
                MsgSeqNum = 1,
                SenderCompId = "MyMarket",
                TargetCompId = "Sender",
                SendingTime = header.SendingTime
            });
            Assert.Equal(logon, logonSend);
        }

        [TestCase("Double Logon")]
        public void DoubleLogon()
        {
            Logon logonSend = new Logon()
            {
                EncryptMethod = EncryptionMethod.None,
                HeartBtInt = 10,
            };
            Send(logonSend);

            var (header1, logon) = Assert.Received<Logon>(Receive());
            Assert.Equal(header1, new Header()
            {
                BeginString = "FIX.4.2",
                BodyLength = 12,
                MsgType = 'A',
                MsgSeqNum = 1,
                SenderCompId = "MyMarket",
                TargetCompId = "Sender",
                SendingTime = header1.SendingTime
            });
            Assert.Equal(logon, logonSend);

            Send(logonSend, 2);

            var (header2, reject) = Assert.Received<SessionReject>(Receive());
            Assert.Equal(header2, new Header()
            {
                BeginString = "FIX.4.2",
                BodyLength = 47,
                MsgType = '3',
                MsgSeqNum = 3,
                SenderCompId = "MyMarket",
                TargetCompId = "Sender",
                SendingTime = header2.SendingTime
            });
            Assert.Equal(reject, new SessionReject()
            {
                RefMsgType = 'A',
                RefSeqNum = 2,
                RefTagId = null,
                SessionRejectReason = RejectReasonSession.InvalidMsgType,
                Text = "User is already logged in"
            });
        }

        [TestCase("Invalid Encryption Method")]
        public void InvalidEncryptionMethod()
        {
            Logon logonSend = new Logon()
            {
                EncryptMethod = (EncryptionMethod)15,
                HeartBtInt = 10,
            };
            Send(logonSend);

            var (header, logon) = Assert.Received<SessionReject>(Receive());
            Assert.Equal(header, new Header()
            {
                BeginString = "FIX.4.2",
                BodyLength = 41,
                MsgType = '3',
                MsgSeqNum = 1,
                SenderCompId = "MyMarket",
                TargetCompId = "",
                SendingTime = header.SendingTime
            });
            Assert.Equal(logon, new SessionReject()
            {
                RefMsgType = 'A',
                RefSeqNum = 0,
                RefTagId = 98,
                SessionRejectReason = RejectReasonSession.IncorrectDataFormat,
                Text = "Value to long"
            });
        }

        [TestCase("Valid Logon - HeartBeart Int lower out of bound")]
        public void ValidLogonLowerBoundHeartBtInt()
        {
            Logon logonSend = new Logon()
            {
                EncryptMethod = EncryptionMethod.None,
                HeartBtInt = 0,
            };
            Send(logonSend);

            var (header, logon) = Assert.Received<Logon>(Receive());
            Assert.Equal(header, new Header()
            {
                BeginString = "FIX.4.2",
                BodyLength = 11,
                MsgType = 'A',
                MsgSeqNum = 1,
                SenderCompId = "MyMarket",
                TargetCompId = "Sender",
                SendingTime = header.SendingTime
            });
            Assert.Equal(logon, new Logon()
            {
                EncryptMethod = EncryptionMethod.None,
                HeartBtInt = 1
            });
        }

        [TestCase("Valid Logon - HeartBeart Int upper out of bound")]
        public void ValidLogonUpperBoundHeartBtInt()
        {
            Logon logonSend = new Logon()
            {
                EncryptMethod = EncryptionMethod.None,
                HeartBtInt = 1000,
            };
            Send(logonSend);

            var (header, logon) = Assert.Received<Logon>(Receive());
            Assert.Equal(header, new Header()
            {
                BeginString = "FIX.4.2",
                BodyLength = 13,
                MsgType = 'A',
                MsgSeqNum = 1,
                SenderCompId = "MyMarket",
                TargetCompId = "Sender",
                SendingTime = header.SendingTime
            });
            Assert.Equal(logon, new Logon()
            {
                EncryptMethod = EncryptionMethod.None,
                HeartBtInt = 100
            });
        }

        [TestInput(EncryptionMethod.DES)]
        [TestInput(EncryptionMethod.PEMDESMD5)]
        [TestInput(EncryptionMethod.PGPDES)]
        [TestInput(EncryptionMethod.PGPDESMD5)]
        [TestInput(EncryptionMethod.PKCS)]
        [TestInput(EncryptionMethod.PKCSDES)]
        [TestCase("(Temporary) Valid Logon - Not Supported Encryption")]
        public void ValidLogonUpperBoundHeartBtInt(EncryptionMethod method)
        {
            Logon logonSend = new Logon()
            {
                EncryptMethod = method,
                HeartBtInt = 1000,
            };
            Send(logonSend);

            var (header, logon) = Assert.Received<SessionReject>(Receive());
            Assert.Equal(header, new Header()
            {
                BeginString = "FIX.4.2",
                BodyLength = 46,
                MsgType = '3',
                MsgSeqNum = 1,
                SenderCompId = "MyMarket",
                TargetCompId = "",
                SendingTime = header.SendingTime
            });
            Assert.Equal(logon, new SessionReject()
            {
                RefSeqNum = 0,
                RefTagId = null,
                RefMsgType = 'A',
                SessionRejectReason = null,
                Text = "Encryption method not supported"
            });
        }
    }
}