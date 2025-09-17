
using System.Net.Sockets;
using System.Text;
using FixGuardian.Message;
using FixGuardian.Messages.Definition;
using FixGuardian.TestFramework.Assertions;
using FixGuardian.TestFramework.Attributes;

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
                MsgType = 'C',
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
            //Assert.Equal(header.MsgType, '8');
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
        }
    }
}