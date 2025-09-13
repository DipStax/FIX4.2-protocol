using FixGuardian.Message;
using System.Net.Sockets;
using System.Text;

namespace FixGuardian
{
    internal class Program
    {
        static void Main(string[] args)
        {
            TcpClient tcpClient = new TcpClient("127.0.0.1", 8080);
            NetworkStream stream = tcpClient.GetStream();

            Header header = new Header();

            header.BeginString = "FIX-4.2";
            header.BodyLength = 0;
            header.MsgType = 'C';
            header.SenderCompId = "AZE";
            header.TargetCompId = "MyMarket";
            header.MsgSeqNum = 1;
            header.SendingTime = DateTime.Now;

            string message = MessageSupport.ToString(header);
            Console.WriteLine(message);
            byte[] data = Encoding.UTF8.GetBytes(message);
            stream.Write(data, 0, data.Length);
        }
    }
}
