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
            string body = FixHelper.ToString(new Logon()
            {
                EncryptMethod = EncryptionMethod.None,
                HeartBtInt = heartbeat,
            });
            string message = FixHelper.AddCheckSum(GetHeader((uint)body.Length).ToString() + body);
            byte[] data = Encoding.UTF8.GetBytes(message);

            TcpStream.Write(data, 0, data.Length);
            SequenceNumber += 1;

            byte[] buffer = new byte[4096];
            int bytesRead = TcpStream.Read(buffer, 0, buffer.Length);

            try
            {
                Assert.Received<Logon>(Encoding.UTF8.GetString(buffer, 0, bytesRead));
            }
            catch (AssertionException ex)
            {
                throw new AssertionException("During logon procesedur", ex);
            }
            IsLoggedIn = true;
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
                MsgSeqNum = SequenceNumber - 1,
                SendingTime = header.SendingTime,
            });
            return message;
        }

        public void AssertServerDisconnet()
        {
            byte[] buffer = new byte[1024];

            int bytesRead =  TcpStream.Read(buffer, 0, buffer.Length);
        }

        private Header GetHeader(uint bodyLength)
        {
            return new Header()
            {
                BeginString = "FIX.4.2",
                BodyLength = bodyLength,
                MsgType = 'A',
                SenderCompId = Name,
                TargetCompId = "MyMarket",
                MsgSeqNum = SequenceNumber,
                SendingTime = DateTime.Now
            };
        }
    }
}