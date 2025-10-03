using System.Net.WebSockets;
using FixGuardian.Messages.Definition;
using FixGuardian.Messages.Enums;
using FixGuardian.TestFramework;
using FixGuardian.TestFramework.Assertions;
using FixGuardian.TestFramework.Attributes;

namespace FixGuardian.Suite
{
    [RequireSuite("NewOrderSingle - Single user")]
    [TestSuite("Cancel validation")]
    public class CancelValidation
    {
        FixClient Client;

        [TestSetup]
        public void Setup()
        {
            Client = new FixClient("Sender");
            Client.Logon();
        }

        [TestTearDown]
        public void TearDown()
        {
            Client.Logout();
        }

        [TestCase("Missing Order Quantity")]
        public void MissingOrderQuantity()
        {
            string guid = Guid.NewGuid().ToString();
            string guidCancel = Guid.NewGuid().ToString();
            Client.Send(new OrderCancelRequest()
            {
                OrigClOrdID = guid,
                ClOrdId = guidCancel,
                Symbol = "Cancel-1",
                Side = TradeSide.Buy,
                TransactTime = DateTime.Now
            });

            BusinessMessageReject reject = Client.Receive<BusinessMessageReject>();
            Assert.Equal(reject, new BusinessMessageReject()
            {
                RefSeqNum = 2,
                RefMsgType = 'F',
                BusinessRejectRefId = guidCancel,
                BusinessRejectReason = RejectReasonBusiness.CondReqFieldMissing,
                Text = "Quantity is required"
            });
        }

        [TestCase("Unknown Order Id")]
        public void UnknownOrderId()
        {
            string guid = Guid.NewGuid().ToString();
            string guidCancel = Guid.NewGuid().ToString();
            Client.Send(new OrderCancelRequest()
            {
                OrigClOrdID = guid,
                ClOrdId = guidCancel,
                Symbol = "Cancel-1",
                OrderQty = 100,
                Side = TradeSide.Buy,
                TransactTime = DateTime.Now
            });

            OrderCancelReject reject = Client.Receive<OrderCancelReject>();
            Assert.Equal(reject, new OrderCancelReject()
            {
                OrderId = "NONE",
                OrigClOrdId = guidCancel,
                OrdStatus = OrderStatus.Rejected,
                CxlRejResponseTo = CancelRejectResponseTo.CancelRequest,
                CxlRejReason = CancelRejectReason.UnknownOrder,
                Text = "Unknow order Id on this side"
            });
        }

        [TestInput(TradeSide.Buy, TradeSide.BuyMinus)]
        [TestInput(TradeSide.BuyMinus, TradeSide.Buy)]
        [TestInput(TradeSide.Sell, TradeSide.SellPlus)]
        // [TestInput(TradeSide.SellPlus, TradeSide.Sell)]
        [TestCase("Not matching order Side")]
        public void WrongOrderSide(TradeSide input, TradeSide cancel)
        {
            string guid = Guid.NewGuid().ToString();
            Client.NewOrder(new NewOrderSingle()
            {
                ClOrdId = guid,
                HandlInst = HandleInstance.Manual,
                Symbol = "Cancel-1",
                Side = input,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 1000
            });

            string guidCancel = Guid.NewGuid().ToString();
            Client.Send(new OrderCancelRequest()
            {
                OrigClOrdID = guid,
                ClOrdId = guidCancel,
                Symbol = "Cancel-1",
                OrderQty = 100,
                Side = cancel,
                TransactTime = DateTime.Now
            });

            OrderCancelReject reject = Client.Receive<OrderCancelReject>();
            Assert.Equal(reject, new OrderCancelReject()
            {
                OrderId = guid,
                OrigClOrdId = guidCancel,
                OrdStatus = OrderStatus.Rejected,
                CxlRejResponseTo = CancelRejectResponseTo.CancelRequest,
                CxlRejReason = CancelRejectReason.UnknownOrder,
                Text = "Side doesn't exactly match"
            });
        }

        [TestCase("Wrong Order Quantity")]
        public void WrongOrderQuantity()
        {
            string guid = Guid.NewGuid().ToString();
            Client.NewOrder(new NewOrderSingle()
            {
                ClOrdId = guid,
                HandlInst = HandleInstance.Manual,
                Symbol = "Cancel-1",
                Side = TradeSide.Sell,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 1000
            });

            string guidCancel = Guid.NewGuid().ToString();
            Client.Send(new OrderCancelRequest()
            {
                OrigClOrdID = guid,
                ClOrdId = guidCancel,
                Symbol = "Cancel-1",
                OrderQty = 100,
                Side = TradeSide.Sell,
                TransactTime = DateTime.Now
            });

            OrderCancelReject reject = Client.Receive<OrderCancelReject>();
            Assert.Equal(reject, new OrderCancelReject()
            {
                OrderId = guid,
                OrigClOrdId = guidCancel,
                OrdStatus = OrderStatus.Rejected,
                CxlRejResponseTo = CancelRejectResponseTo.CancelRequest,
                CxlRejReason = CancelRejectReason.TooLate,
                Text = "Order has invalid quantity"
            });
        }

        [TestCase("Valid Cancel Request")]
        public void ValidCancelRequest()
        {
            string guid = Guid.NewGuid().ToString();
            Client.NewOrder(new NewOrderSingle()
            {
                ClOrdId = guid,
                HandlInst = HandleInstance.Manual,
                Symbol = "Cancel-1",
                Side = TradeSide.Sell,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 1000
            });

            string guidCancel = Guid.NewGuid().ToString();
            Client.Send(new OrderCancelRequest()
            {
                OrigClOrdID = guid,
                ClOrdId = guidCancel,
                Symbol = "Cancel-1",
                OrderQty = 1000,
                Side = TradeSide.Sell,
                TransactTime = DateTime.Now
            });

            Console.WriteLine("before receive reject");
            ExecutionReport reject = Client.Receive<ExecutionReport>();
            Assert.Equal(reject, new ExecutionReport()
            {
                OrderID = guid,
                ClOrdID = guidCancel,
                ExecId = reject.ExecId,
                ExecTransType = TransactionType.Cancel,
                ExecType = ExecutionType.Canceled,
                OrdStatus = OrderStatus.New,
                Symbol = "Cancel-1",
                Side = TradeSide.Sell,
                LeavesQty = 0,
                CumQty = 0,
                AvgPx = 0
            });
        }
    }
}