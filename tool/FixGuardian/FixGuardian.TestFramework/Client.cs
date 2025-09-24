using System.Net.Sockets;
using System.Reflection.Metadata.Ecma335;
using FixGuardian.Messages;
using FixGuardian.Messages.Enums;
using FixGuardian.Messages.Definition;
using System.Text;
using FixGuardian.TestFramework.Assertions;

namespace FixGuardian.TestFramework
{
    public class FixClient
    {
        private TcpClient TcpSocket { get; }
        public NetworkStream TcpStream { get; }

        public string Name { get; }
        public uint SequenceNumber { get; set; }

        public bool IsLoggedIn { get; private set; } = false;

        private StringBuilder Buffer { get; set; } = new StringBuilder();

        public FixClient(string name, uint seqnum = 0)
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

        public void Logout()
        {
            Send(new Logout());

            try
            {
                Receive<Logout>();
            }
            catch (AssertionException ex)
            {
                throw new AssertionException("During receive of logout", ex);
            }
            TcpStream.Close();
            TcpSocket.Close();
        }

        public ExecutionReport NewOrder(NewOrderSingle order)
        {
            ExecutionReport execReport;

            Send(order);

            try
            {
                execReport = Receive<ExecutionReport>();

                Assert.Equal(execReport, new ExecutionReport()
                {
                    OrderID = order.ClOrdId,
                    ExecId = execReport.ExecId,
                    ExecTransType = TransactionType.New,
                    ExecType = ExecutionType.New,
                    OrdStatus = OrderStatus.New,
                    Symbol = order.Symbol,
                    Side = order.Side,
                    OrderQty = order.OrderQty,
                    OrdType = order.OrdType,
                    Price = order.Price,
                    LeavesQty = order.OrderQty,
                    LastShares = 0,
                    LastPx = 0,
                    CumQty = 0,
                    AvgPx = 0
                });

            }
            catch (AssertionException ex)
            {
                throw new AssertionException("During execution report receive from new order", ex);
            }
            return execReport;
        }

        public void Send<T>(T message)
            where T : AMessage, IMessage, new()
        {
            string body = FixHelper.ToString(message);
            string messageStr = FixHelper.AddCheckSum(GetHeader((uint)body.Length, message.MsgType).ToString() + body);

            Console.WriteLine($"Sending: '{messageStr.Replace('\u0001', '^')}'");

            byte[] data = Encoding.UTF8.GetBytes(messageStr);

            TcpStream.Write(data, 0, data.Length);
            SequenceNumber++;
        }

        public T Receive<T>()
            where T : AMessage, new()
        {
            while (true)
            {
                // Try to extract a complete message from buffer
                var msg = TryExtractMessage();
                if (msg != null)
                {
                    Console.WriteLine($"Receiving: '{msg.Replace('\u0001', '^')}'");

                    var (header, message) = Assert.Received<T>(msg);
                    Assert.Equal(header, new Header()
                    {
                        BeginString = "FIX.4.2",
                        BodyLength = header.BodyLength,
                        MsgType = header.MsgType,
                        SenderCompId = "MyMarket",
                        TargetCompId = Name,
                        MsgSeqNum = SequenceNumber,
                        SendingTime = header.SendingTime,
                    });
                    SequenceNumber++;
                    return message;
                }

                byte[] buffer = new byte[4096];
                int bytesRead = TcpStream.Read(buffer, 0, buffer.Length);

                if (bytesRead == 0)
                    throw new AssertionException("Connection closed by server");
                Buffer.Append(Encoding.UTF8.GetString(buffer, 0, bytesRead));
            }
        }

        public bool IsConnected()
        {
            Socket socket = TcpSocket.Client;

            return socket.Poll(0, SelectMode.SelectRead) && socket.Available != 0;
        }

        public void AssertServerDisconnet()
        {
            if (!IsConnected())
                return;
            throw new AssertionException("Client not disconenct");
        }

        public void AssertNoMoreMessage()
        {
            try
            {
                Socket socket = TcpSocket.Client;
                if (Buffer.Length > 0)
                    throw new AssertionException("Buffer not empty");
                if (socket.Available != 0)
                    throw new AssertionException("Still pending data in network socket");
            }
            catch (AssertionException ex)
            {
                throw new AssertionException("During verification of no more message", ex);
            }
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

        private string? TryExtractMessage()
        {
            string buf = Buffer.ToString();

            int checksumIndex = buf.IndexOf("\u000110=", StringComparison.Ordinal);

            if (checksumIndex == -1)
                return null;

            int endIndex = buf.IndexOf('\u0001', checksumIndex + 4);

            if (endIndex == -1)
                return null;
            Buffer.Remove(0, endIndex + 1);
            return buf[..(endIndex + 1)];
        }
    }
}