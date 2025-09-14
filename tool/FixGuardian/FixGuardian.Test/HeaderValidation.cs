using System.Net.Sockets;
using System.Text;
using FixGuardian.Message;
using FixGuardian.TestFramework.Attributes;

namespace FixGuardian.Test
{
    [TestSuite("Header validation")]
    public class HeaderValidation
    {
        [TestCase("Invalid first positional")]
        public void InvalidFirstPosition()
        {
            TcpClient tcpClient = new TcpClient("127.0.0.1", 8080);
            NetworkStream stream = tcpClient.GetStream();

            Header header = new Header()
            {
                BeginString = null,
                BodyLength = 0,
                MsgType = 'C',
                SenderCompId = "AZE",
                TargetCompId = "MyMarket",
                MsgSeqNum = 1,
                SendingTime = DateTime.Now
            };
            byte[] data = Encoding.UTF8.GetBytes(MessageSupport.ToString(header, true));
            stream.Write(data, 0, data.Length);
            byte[] buffer = new byte[4096];
            int bytesRead = stream.Read(buffer, 0, buffer.Length);
            Console.WriteLine(Encoding.UTF8.GetString(buffer, 0, bytesRead));
        }
    }

}
