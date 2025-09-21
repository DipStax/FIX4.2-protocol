using FixGuardian.Messages.Definition;
using FixGuardian.Messages.Enums;
using FixGuardian.TestFramework;
using FixGuardian.TestFramework.Assertions;
using FixGuardian.TestFramework.Attributes;
using System;

namespace FixGuardian.Suite
{
    // [RequireSuite("Logon validation")]
    [TestSuite("NewOrderSingle - Single user")]
    public class NewOrderSingleSingleUser
    {
        public FixClient Client;

        [TestSetup]
        public void Setup()
        {
            Client = new FixClient("Market1");
            Client.Logon();
        }

        [TestTearDown]
        public void TearDown()
        {
            Client.Logout();
        }

        [TestCase("Aknowledge order")]
        public void AknowledgeOrder()
        {
            string guid = Guid.NewGuid().ToString();

            Client.Send(new NewOrderSingle()
            {
                ClOrdId = guid,
                HandlInst = HandleInstance.Manual,
                Symbol = "SellPlus",
                Side = TradeSide.SellPlus,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 100
            });
            ExecutionReport execReport = Client.Receive<ExecutionReport>();

            Assert.Equal(execReport, new ExecutionReport()
            {
                OrderID = guid,
                ExecId = execReport.ExecId,
                ExecTransType = TransactionType.New,
                ExecType = ExecutionType.New,
                OrdStatus = OrderStatus.New,
                Symbol = "SellPlus",
                Side = TradeSide.SellPlus,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 100,
                LeavesQty = 1000,
                CumQty = 0,
                AvgPx = 0
            });
        }

        [TestCase("Unknow Symbol")]
        public void UnknownSymbol()
        {
            string guid = Guid.NewGuid().ToString();

            Client.Send(new NewOrderSingle()
            {
                ClOrdId = guid,
                HandlInst = HandleInstance.Manual,
                Symbol = "Unknown",
                Side = TradeSide.SellPlus,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 100
            });
            BusinessMessageReject reject = Client.Receive<BusinessMessageReject>();
            Assert.Equal(reject, new BusinessMessageReject()
            {
                RefSeqNum = 2,
                RefMsgType = 'D',
                BusinessRejectReason = RejectReasonBusiness.UnknownSecurity,
                Text = "Unknown symbol"
            });
        }

        [TestCase("Same ClOrdId")]
        public void SameCallerOrderId()
        {
            string guid = Guid.NewGuid().ToString();

            Client.Send(new NewOrderSingle()
            {
                ClOrdId = guid,
                HandlInst = HandleInstance.Manual,
                Symbol = "SellPlus",
                Side = TradeSide.SellPlus,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 100
            });
            ExecutionReport execReport = Client.Receive<ExecutionReport>();

            Assert.Equal(execReport, new ExecutionReport()
            {
                OrderID = guid,
                ExecId = execReport.ExecId,
                ExecTransType = TransactionType.New,
                ExecType = ExecutionType.New,
                OrdStatus = OrderStatus.New,
                Symbol = "SellPlus",
                Side = TradeSide.SellPlus,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 100,
                LeavesQty = 1000,
                CumQty = 0,
                AvgPx = 0
            });

            Client.Send(new NewOrderSingle()
            {
                ClOrdId = guid,
                HandlInst = HandleInstance.Manual,
                Symbol = "SellPlus",
                Side = TradeSide.SellPlus,
                OrderQty = 2000,
                OrdType = OrderType.Limit,
                Price = 100
            });
            BusinessMessageReject reject = Client.Receive<BusinessMessageReject>();
            Assert.Equal(reject, new BusinessMessageReject()
            {
                RefSeqNum = 4,
                RefMsgType = 'D',
                BusinessRejectRefId = guid,
                BusinessRejectReason = RejectReasonBusiness.Other,
                Text = "Order Id already used"
            });

        }

        // [TestInput(HandleInstance.AutoPrivate)]
        // [TestInput(HandleInstance.AutoPublic)]
        // [TestCase("(Temporary) Not supported Handle Instance")]
        // public void NotSupportedHandleInstance(HandleInstance instance)
        // {
        //     string guid = Guid.NewGuid().ToString();

        //     Client.Send(new NewOrderSingle()
        //     {
        //         ClOrdId = guid,
        //         HandlInst = instance,
        //         Symbol = "SellPlus",
        //         Side = TradeSide.SellPlus,
        //         OrderQty = 1000,
        //         OrdType = OrderType.Limit,
        //         Price = 100
        //     });

        //     SessionReject reject = Client.Receive<SessionReject>();
        // }

        // [TestInput(TradeSide.Buy)]
        // [TestInput(TradeSide.Cross)]
        // [TestInput(TradeSide.CrossShort)]
        // [TestInput(TradeSide.Sell)]
        // [TestInput(TradeSide.SellShort)]
        // [TestInput(TradeSide.SellShortExempt)]
        // [TestInput(TradeSide.Undisclosed)]
        // [TestCase("(Temporary) Not supported Side")]
        // public void NotSupportedSide(TradeSide side)
        // {
        //     string guid = Guid.NewGuid().ToString();

        //     Client.Send(new NewOrderSingle()
        //     {
        //         ClOrdId = guid,
        //         HandlInst = HandleInstance.Manual,
        //         Symbol = "SellPlus",
        //         Side = side,
        //         OrderQty = 1000,
        //         OrdType = OrderType.Limit,
        //         Price = 100
        //     });

        //     SessionReject reject = Client.Receive<SessionReject>();
        // }
    }
}