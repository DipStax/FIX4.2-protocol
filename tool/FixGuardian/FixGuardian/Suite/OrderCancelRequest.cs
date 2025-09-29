using FixGuardian.Messages.Definition;
using FixGuardian.Messages.Enums;
using FixGuardian.TestFramework;
using FixGuardian.TestFramework.Assertions;
using FixGuardian.TestFramework.Attributes;

namespace FixGuardian.Suite
{
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
    }
}