
using System.Net.Sockets;
using System.Reflection.Metadata.Ecma335;
using System.Text;
using FixGuardian.Message;
using FixGuardian.TestFramework.Attributes;

namespace FixGuardian.Suite
{
    [TestSuite("Header validation")]
    public class HeaderValidation
    {
        [TestCase("Invalid first positional")]
        public void InvalidFirstPosition()
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

            string headerStr = header1.ToString(true);

            Console.WriteLine($"Sending: '{headerStr.Replace('\u0001', '^')}'");

            byte[] data = Encoding.UTF8.GetBytes(headerStr);
            stream.Write(data, 0, data.Length);

            // Console.WriteLine($"header is same: {headerStr2 == headerStr}");

            // string header1str = FixHelper.ToString(header1, true);
            // Console.WriteLine(header1str);
            // FixHelper.ToMap(header1str);
            // FixHelper.FromString<Header>(header1str);
            // // Assert.Equal(header1, header2);

            // SessionReject reject = new SessionReject()
            // {
            //     SessionRejectReason = (RejectReasonSession)12
            // };
            // Console.WriteLine(FixHelper.ToString(reject, true));


            byte[] buffer = new byte[4096];
            int bytesRead = stream.Read(buffer, 0, buffer.Length);
            Header receiveHeader = new Header();

            string receiveBuffer = Encoding.UTF8.GetString(buffer, 0, bytesRead);
            Console.WriteLine(receiveBuffer);

            List<KeyValuePair<ushort, string>> msgmap = FixHelper.ToMap(receiveBuffer);

            receiveHeader.FromString(msgmap, true);

            // Console.WriteLine(Encoding.UTF8.GetString(buffer, 0, bytesRead));
        }
    }
}