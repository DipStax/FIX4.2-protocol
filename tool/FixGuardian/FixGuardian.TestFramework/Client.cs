using System.Net.Sockets;
using System.Reflection.Metadata.Ecma335;
using FixGuardian.Messages;
using FixGuardian.Messages.Enums;
using FixGuardian.Messages.Definition;
using System.Text;
using FixGuardian.TestFramework.Assertions;

namespace FixGuardian.TestFramework
{
    public class Client
    {
        private TcpClient TcpSocket { get; }
        public NetworkStream TcpStream { get; }

        public string Name { get; }
        public uint SequenceNumber { get; set; }

        public bool IsLoggedIn { get; private set; } = false;

        public Client(string name, uint seqnum = 0)
        {
            TcpSocket = new TcpClient("127.0.0.1", 8080);
            TcpStream = TcpSocket.GetStream();

            Name = name;
            SequenceNumber = seqnum;
        }

        public void Logon(uint heartbeat = 100)
        {
            Logon send_logon = new Logon()
            {
                EncryptMethod = EncryptionMethod.None,
                HeartBtInt = heartbeat,
            };
            Send(send_logon);

            try
            {
                Logon logon = Receive<Logon>();
                Assert.Equal(logon, send_logon);
            }
            catch (AssertionException ex)
            {
                throw new AssertionException("During receive of Logon", ex);
            }
            IsLoggedIn = true;
        }

        public void Send<T>(T message)
            where T : AMessage, IMessage, new()
        {
            string body = FixHelper.ToString(message);
            string messageStr = FixHelper.AddCheckSum(GetHeader((uint)body.Length, message.MsgType).ToString() + body);
            byte[] data = Encoding.UTF8.GetBytes(messageStr);

            TcpStream.Write(data, 0, data.Length);
            SequenceNumber++;
        }

        public T Receive<T>()
            where T : AMessage, new()
        {
            byte[] buffer = new byte[4096];
            int bytesRead = TcpStream.Read(buffer, 0, buffer.Length);

            var (header, message) = Assert.Received<T>(Encoding.UTF8.GetString(buffer, 0, bytesRead));
            Assert.Equal(header, new Header()
            {
                BeginString = "FIX.4.2",
                BodyLength = header.BodyLength,
                MsgType = header.MsgType,
                SenderCompId = "MyMarket",
                TargetCompId = Name,
                MsgSeqNum = SequenceNumber ,
                SendingTime = header.SendingTime,
            });
            SequenceNumber++;
            return message;
        }

        public void AssertServerDisconnet()
        {
            Socket socket = TcpSocket.Client;

            if (socket.Poll(100_000, SelectMode.SelectRead) && socket.Available == 0)
                return;
            throw new AssertionException("Client not disconenct");
        }

        private Header GetHeader(uint bodyLength, char msgType)
        {
            return new Header()
            {
                BeginString = "FIX.4.2",
                BodyLength = bodyLength,
                MsgType = msgType,
                SenderCompId = Name,
                TargetCompId = "MyMarket",
                MsgSeqNum = SequenceNumber,
                SendingTime = DateTime.Now
            };
        }
    }
}