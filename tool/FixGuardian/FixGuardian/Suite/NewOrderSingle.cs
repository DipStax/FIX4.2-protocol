using FixGuardian.Messages.Definition;
using FixGuardian.Messages.Enums;
using FixGuardian.TestFramework;
using FixGuardian.TestFramework.Assertions;
using FixGuardian.TestFramework.Attributes;
using System;

namespace FixGuardian.Suite
{
    // [RequireSuite("Logon validation")]
    [TestSuite("NewOrderSingle - Conditional invalid")]
    public class NewOrderSingleConditionalInvalid
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

        [TestCase("No price")]
        public void OptionalPrice()
        {
            string guid = Guid.NewGuid().ToString();

            Client.Send(new NewOrderSingle()
            {
                ClOrdId = guid,
                HandlInst = HandleInstance.Manual,
                Symbol = "Test-Invalid-Trade",
                Side = TradeSide.SellPlus,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
            });
            BusinessMessageReject reject = Client.Receive<BusinessMessageReject>();
            Assert.Equal(reject, new BusinessMessageReject()
            {
                RefSeqNum = 2,
                RefMsgType = 'D',
                BusinessRejectRefId = guid,
                BusinessRejectReason = RejectReasonBusiness.CondReqFieldMissing,
                Text = "Price required when OrderType is Limit"
            });
        }

        [TestCase("No Order Quantity")]
        public void OptionalOrderQuantity()
        {
            string guid = Guid.NewGuid().ToString();

            Client.Send(new NewOrderSingle()
            {
                ClOrdId = guid,
                HandlInst = HandleInstance.Manual,
                Symbol = "Test-Invalid-Trade",
                Side = TradeSide.SellPlus,
                OrdType = OrderType.Limit,
                Price = 100
            });
            BusinessMessageReject reject = Client.Receive<BusinessMessageReject>();
            Assert.Equal(reject, new BusinessMessageReject()
            {
                RefSeqNum = 2,
                RefMsgType = 'D',
                BusinessRejectRefId = guid,
                BusinessRejectReason = RejectReasonBusiness.CondReqFieldMissing,
                Text = "Order quantity required"
            });
        }

        [TestInput(OrderType.ForexLimit)]
        [TestInput(OrderType.ForexMarket)]
        [TestInput(OrderType.ForexPrevQuoted)]
        [TestInput(OrderType.ForexSwap)]
        [TestInput(OrderType.Funari)]
        [TestInput(OrderType.LimitOnClose)]
        [TestInput(OrderType.LimitOrBetter)]
        [TestInput(OrderType.LimitWithOrWithout)]
        [TestInput(OrderType.Market)]
        [TestInput(OrderType.MarketOnClose)]
        [TestInput(OrderType.OnBasis)]
        [TestInput(OrderType.OnClose)]
        [TestInput(OrderType.Pegged)]
        [TestInput(OrderType.PrevIndicated)]
        [TestInput(OrderType.PrevQuoted)]
        [TestInput(OrderType.Stop)]
        [TestInput(OrderType.StopLimit)]
        [TestInput(OrderType.WithOrWithout)]
        [TestCase("(Temporary) Not supported Order Type")]
        public void NotSupportedOrderType(OrderType orderType)
        {
            string guid = Guid.NewGuid().ToString();

            Client.Send(new NewOrderSingle()
            {
                ClOrdId = guid,
                HandlInst = HandleInstance.Manual,
                Symbol = "Test-Invalid-Trade",
                Side = TradeSide.SellPlus,
                OrderQty = 1000,
                OrdType = orderType,
                Price = 100
            });

            BusinessMessageReject reject = Client.Receive<BusinessMessageReject>();
            Assert.Equal(reject, new BusinessMessageReject()
            {
                RefSeqNum = 2,
                RefMsgType = 'D',
                BusinessRejectRefId = guid,
                BusinessRejectReason = RejectReasonBusiness.Other,
                Text = "Not supported order type"
            });
        }


        [TestInput(TradeSide.Cross)]
        [TestInput(TradeSide.CrossShort)]
        [TestInput(TradeSide.SellShort)]
        [TestInput(TradeSide.SellShortExempt)]
        [TestInput(TradeSide.Undisclosed)]
        [TestCase("(Temporary) Not supported Side")]
        public void NotSupportedSide(TradeSide side)
        {
            string guid = Guid.NewGuid().ToString();

            Client.Send(new NewOrderSingle()
            {
                ClOrdId = guid,
                HandlInst = HandleInstance.Manual,
                Symbol = "Test-Invalid-Trade",
                Side = side,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 100
            });

            BusinessMessageReject reject = Client.Receive<BusinessMessageReject>();
            Assert.Equal(reject, new BusinessMessageReject()
            {
                RefSeqNum = 2,
                RefMsgType = 'D',
                BusinessRejectRefId = guid,
                BusinessRejectReason = RejectReasonBusiness.Other,
                Text = "Not supported side"
            });
        }
    }

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

        [TestCase("Aknowledge order - Sell Side")]
        public void AknowledgeOrder()
        {
            string guid = Guid.NewGuid().ToString();

            Client.Send(new NewOrderSingle()
            {
                ClOrdId = guid,
                HandlInst = HandleInstance.Manual,
                Symbol = "Sell-1",
                Side = TradeSide.Sell,
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
                Symbol = "Sell-1",
                Side = TradeSide.Sell,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 100,
                LeavesQty = 1000,
                LastShares = 0,
                LastPx = 0,
                CumQty = 0,
                AvgPx = 0
            });
        }

        [TestCase("Aknowledge order - Buy side")]
        public void AknowledgeOrderBuy()
        {
            string guid = Guid.NewGuid().ToString();

            Client.Send(new NewOrderSingle()
            {
                ClOrdId = guid,
                HandlInst = HandleInstance.Manual,
                Symbol = "Buy-1",
                Side = TradeSide.Buy,
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
                Symbol = "Buy-1",
                Side = TradeSide.Buy,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 100,
                LeavesQty = 1000,
                LastShares = 0,
                LastPx = 0,
                CumQty = 0,
                AvgPx = 0
            });
        }

        [TestCase("Aknowledge order - BuyMinus side")]
        public void AknowledgeOrderBuyMinus()
        {
            string guid = Guid.NewGuid().ToString();

            Client.Send(new NewOrderSingle()
            {
                ClOrdId = guid,
                HandlInst = HandleInstance.Manual,
                Symbol = "BuyMinus-1",
                Side = TradeSide.BuyMinus,
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
                Symbol = "BuyMinus-1",
                Side = TradeSide.BuyMinus,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 100,
                LeavesQty = 1000,
                LastShares = 0,
                LastPx = 0,
                CumQty = 0,
                AvgPx = 0
            });
        }

        [TestCase("Wrong Tick Side - SellPlus side")]
        public void WrongTickSideSellPlus()
        {
            string guid = Guid.NewGuid().ToString();

            Client.Send(new NewOrderSingle()
            {
                ClOrdId = guid,
                HandlInst = HandleInstance.Manual,
                Symbol = "SellPlus-1",
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
                ExecType = ExecutionType.Rejected,
                OrdStatus = OrderStatus.Rejected,
                Symbol = "SellPlus-1",
                Side = TradeSide.SellPlus,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 100,
                LeavesQty = 0,
                LastShares = 0,
                LastPx = 0,
                CumQty = 0,
                AvgPx = 0,
                Text = "Invalid tick direction for selected side"
            });
        }
    }

    [TestSuite("NewOrderSingle - Double user")]
    public class NewOrderSingleDoubleUser
    {
        public FixClient Client1;
        public FixClient Client2;

        [TestSetup]
        public void Setup()
        {
            Client1 = new FixClient("Market1");
            Client1.Logon();

            Client2 = new FixClient("Market2");
            Client2.Logon();
        }

        [TestTearDown]
        public void TearDown()
        {
            Client1.Logout();
            Client2.Logout();
        }

        [TestCase("Sell then Buy - Same Price")]
        public void SellThenBuySamePrice()
        {
            string guid1 = Guid.NewGuid().ToString();

            Client1.Send(new NewOrderSingle()
            {
                ClOrdId = guid1,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 100
            });

            ExecutionReport akReport1 = Client1.Receive<ExecutionReport>();

            Assert.Equal(akReport1, new ExecutionReport()
            {
                OrderID = guid1,
                ExecId = akReport1.ExecId,
                ExecTransType = TransactionType.New,
                ExecType = ExecutionType.New,
                OrdStatus = OrderStatus.New,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 100,
                LeavesQty = 1000,
                LastShares = 0,
                LastPx = 0,
                CumQty = 0,
                AvgPx = 0
            });

            string guid2 = Guid.NewGuid().ToString();

            Client2.Send(new NewOrderSingle()
            {
                ClOrdId = guid2,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Buy,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 100
            });

            ExecutionReport akReport2 = Client2.Receive<ExecutionReport>();

            Assert.Equal(akReport2, new ExecutionReport()
            {
                OrderID = guid2,
                ExecId = akReport2.ExecId,
                ExecTransType = TransactionType.New,
                ExecType = ExecutionType.New,
                OrdStatus = OrderStatus.New,
                Symbol = "Trade-1",
                Side = TradeSide.Buy,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 100,
                LeavesQty = 1000,
                LastShares = 0,
                LastPx = 0,
                CumQty = 0,
                AvgPx = 0
            });

            ExecutionReport orderReport1 = Client1.Receive<ExecutionReport>();

            Assert.Equal(orderReport1, new ExecutionReport()
            {
                OrderID = guid1,
                ExecId = orderReport1.ExecId,
                ExecTransType = TransactionType.New,
                ExecType = ExecutionType.Filled,
                OrdStatus = OrderStatus.New,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                LeavesQty = 0,
                LastShares = 1000,
                LastPx = 100,
                CumQty = 1000,
                AvgPx = 100
            });

            ExecutionReport orderReport2 = Client2.Receive<ExecutionReport>();

            Assert.Equal(orderReport2, new ExecutionReport()
            {
                OrderID = guid2,
                ExecId = orderReport2.ExecId,
                ExecTransType = TransactionType.New,
                ExecType = ExecutionType.Filled,
                OrdStatus = OrderStatus.New,
                Symbol = "Trade-1",
                Side = TradeSide.Buy,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                LeavesQty = 0,
                LastShares = 1000,
                LastPx = 100,
                CumQty = 1000,
                AvgPx = 100
            });
        }

        [TestCase("Buy then Sell - Same Price")]
        public void BuyThenSell()
        {
            string guid1 = Guid.NewGuid().ToString();

            Client1.Send(new NewOrderSingle()
            {
                ClOrdId = guid1,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Buy,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 100
            });

            ExecutionReport akReport1 = Client1.Receive<ExecutionReport>();

            Assert.Equal(akReport1, new ExecutionReport()
            {
                OrderID = guid1,
                ExecId = akReport1.ExecId,
                ExecTransType = TransactionType.New,
                ExecType = ExecutionType.New,
                OrdStatus = OrderStatus.New,
                Symbol = "Trade-1",
                Side = TradeSide.Buy,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 100,
                LeavesQty = 1000,
                LastShares = 0,
                LastPx = 0,
                CumQty = 0,
                AvgPx = 0
            });

            string guid2 = Guid.NewGuid().ToString();

            Client2.Send(new NewOrderSingle()
            {
                ClOrdId = guid2,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 100
            });

            ExecutionReport akReport2 = Client2.Receive<ExecutionReport>();

            Assert.Equal(akReport2, new ExecutionReport()
            {
                OrderID = guid2,
                ExecId = akReport2.ExecId,
                ExecTransType = TransactionType.New,
                ExecType = ExecutionType.New,
                OrdStatus = OrderStatus.New,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 100,
                LeavesQty = 1000,
                LastShares = 0,
                LastPx = 0,
                CumQty = 0,
                AvgPx = 0
            });

            ExecutionReport orderReport1 = Client1.Receive<ExecutionReport>();

            Assert.Equal(orderReport1, new ExecutionReport()
            {
                OrderID = guid1,
                ExecId = orderReport1.ExecId,
                ExecTransType = TransactionType.New,
                ExecType = ExecutionType.Filled,
                OrdStatus = OrderStatus.New,
                Symbol = "Trade-1",
                Side = TradeSide.Buy,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                LeavesQty = 0,
                LastShares = 1000,
                LastPx = 100,
                CumQty = 1000,
                AvgPx = 100
            });

            ExecutionReport orderReport2 = Client2.Receive<ExecutionReport>();

            Assert.Equal(orderReport2, new ExecutionReport()
            {
                OrderID = guid2,
                ExecId = orderReport2.ExecId,
                ExecTransType = TransactionType.New,
                ExecType = ExecutionType.Filled,
                OrdStatus = OrderStatus.New,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                LeavesQty = 0,
                LastShares = 1000,
                LastPx = 100,
                CumQty = 1000,
                AvgPx = 100
            });
        }

        [TestCase("Sell then Buy - Higher Price")]
        public void SellThenBuyHigher()
        {
            string guid1 = Guid.NewGuid().ToString();

            Client1.Send(new NewOrderSingle()
            {
                ClOrdId = guid1,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 100
            });
            ExecutionReport akReport1 = Client1.Receive<ExecutionReport>();
            Assert.Equal(akReport1, new ExecutionReport()
            {
                OrderID = guid1,
                ExecId = akReport1.ExecId,
                ExecTransType = TransactionType.New,
                ExecType = ExecutionType.New,
                OrdStatus = OrderStatus.New,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 100,
                LeavesQty = 1000,
                LastShares = 0,
                LastPx = 0,
                CumQty = 0,
                AvgPx = 0
            });

            string guid2 = Guid.NewGuid().ToString();
            Client2.Send(new NewOrderSingle()
            {
                ClOrdId = guid2,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Buy,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 200
            });
            ExecutionReport akReport2 = Client2.Receive<ExecutionReport>();
            Assert.Equal(akReport2, new ExecutionReport()
            {
                OrderID = guid2,
                ExecId = akReport2.ExecId,
                ExecTransType = TransactionType.New,
                ExecType = ExecutionType.New,
                OrdStatus = OrderStatus.New,
                Symbol = "Trade-1",
                Side = TradeSide.Buy,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 200,
                LeavesQty = 1000,
                LastShares = 0,
                LastPx = 0,
                CumQty = 0,
                AvgPx = 0
            });

            ExecutionReport orderReport1 = Client1.Receive<ExecutionReport>();
            Assert.Equal(orderReport1, new ExecutionReport()
            {
                OrderID = guid1,
                ExecId = orderReport1.ExecId,
                ExecTransType = TransactionType.New,
                ExecType = ExecutionType.Filled,
                OrdStatus = OrderStatus.New,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                LeavesQty = 0,
                LastShares = 1000,
                LastPx = 100,
                CumQty = 1000,
                AvgPx = 100
            });

            ExecutionReport orderReport2 = Client2.Receive<ExecutionReport>();
            Assert.Equal(orderReport2, new ExecutionReport()
            {
                OrderID = guid2,
                ExecId = orderReport2.ExecId,
                ExecTransType = TransactionType.New,
                ExecType = ExecutionType.Filled,
                OrdStatus = OrderStatus.New,
                Symbol = "Trade-1",
                Side = TradeSide.Buy,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                LeavesQty = 0,
                LastShares = 1000,
                LastPx = 100,
                CumQty = 1000,
                AvgPx = 100
            });
        }

        [TestCase("Buy then Sell - Lower Price")]
        public void BuyThenSellLower()
        {
            string guid1 = Guid.NewGuid().ToString();

            Client1.Send(new NewOrderSingle()
            {
                ClOrdId = guid1,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Buy,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 200
            });
            ExecutionReport akReport1 = Client1.Receive<ExecutionReport>();
            Assert.Equal(akReport1, new ExecutionReport()
            {
                OrderID = guid1,
                ExecId = akReport1.ExecId,
                ExecTransType = TransactionType.New,
                ExecType = ExecutionType.New,
                OrdStatus = OrderStatus.New,
                Symbol = "Trade-1",
                Side = TradeSide.Buy,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 200,
                LeavesQty = 1000,
                LastShares = 0,
                LastPx = 0,
                CumQty = 0,
                AvgPx = 0
            });

            string guid2 = Guid.NewGuid().ToString();
            Client2.Send(new NewOrderSingle()
            {
                ClOrdId = guid2,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 100
            });
            ExecutionReport akReport2 = Client2.Receive<ExecutionReport>();
            Assert.Equal(akReport2, new ExecutionReport()
            {
                OrderID = guid2,
                ExecId = akReport2.ExecId,
                ExecTransType = TransactionType.New,
                ExecType = ExecutionType.New,
                OrdStatus = OrderStatus.New,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 100,
                LeavesQty = 1000,
                LastShares = 0,
                LastPx = 0,
                CumQty = 0,
                AvgPx = 0
            });

            ExecutionReport orderReport1 = Client1.Receive<ExecutionReport>();
            Assert.Equal(orderReport1, new ExecutionReport()
            {
                OrderID = guid1,
                ExecId = orderReport1.ExecId,
                ExecTransType = TransactionType.New,
                ExecType = ExecutionType.Filled,
                OrdStatus = OrderStatus.New,
                Symbol = "Trade-1",
                Side = TradeSide.Buy,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                LeavesQty = 0,
                LastShares = 1000,
                LastPx = 100,
                CumQty = 1000,
                AvgPx = 100
            });

            ExecutionReport orderReport2 = Client2.Receive<ExecutionReport>();
            Assert.Equal(orderReport2, new ExecutionReport()
            {
                OrderID = guid2,
                ExecId = orderReport2.ExecId,
                ExecTransType = TransactionType.New,
                ExecType = ExecutionType.Filled,
                OrdStatus = OrderStatus.New,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                LeavesQty = 0,
                LastShares = 1000,
                LastPx = 100,
                CumQty = 1000,
                AvgPx = 100
            });
        }

        [TestCase("Sell then Buy - Double not same - All")]
        public void DoubleNotSamePriceSellThenBuyAll()
        {
            string guid1_1 = Guid.NewGuid().ToString();

            Client1.Send(new NewOrderSingle()
            {
                ClOrdId = guid1_1,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 100
            });
            ExecutionReport akReport1_1 = Client1.Receive<ExecutionReport>();
            Assert.Equal(akReport1_1, new ExecutionReport()
            {
                OrderID = guid1_1,
                ExecId = akReport1_1.ExecId,
                ExecTransType = TransactionType.New,
                ExecType = ExecutionType.New,
                OrdStatus = OrderStatus.New,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 100,
                LeavesQty = 1000,
                LastShares = 0,
                LastPx = 0,
                CumQty = 0,
                AvgPx = 0
            });

            string guid1_2 = Guid.NewGuid().ToString();
            Client1.Send(new NewOrderSingle()
            {
                ClOrdId = guid1_2,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 150
            });
            ExecutionReport akReport1_2 = Client1.Receive<ExecutionReport>();
            Assert.Equal(akReport1_2, new ExecutionReport()
            {
                OrderID = guid1_2,
                ExecId = akReport1_2.ExecId,
                ExecTransType = TransactionType.New,
                ExecType = ExecutionType.New,
                OrdStatus = OrderStatus.New,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 150,
                LeavesQty = 1000,
                LastShares = 0,
                LastPx = 0,
                CumQty = 0,
                AvgPx = 0
            });

            string guid2 = Guid.NewGuid().ToString();
            Client2.Send(new NewOrderSingle()
            {
                ClOrdId = guid2,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Buy,
                OrderQty = 2000,
                OrdType = OrderType.Limit,
                Price = 200
            });
            ExecutionReport akReport2 = Client2.Receive<ExecutionReport>();
            Assert.Equal(akReport2, new ExecutionReport()
            {
                OrderID = guid2,
                ExecId = akReport2.ExecId,
                ExecTransType = TransactionType.New,
                ExecType = ExecutionType.New,
                OrdStatus = OrderStatus.New,
                Symbol = "Trade-1",
                Side = TradeSide.Buy,
                OrderQty = 2000,
                OrdType = OrderType.Limit,
                Price = 200,
                LeavesQty = 2000,
                LastShares = 0,
                LastPx = 0,
                CumQty = 0,
                AvgPx = 0
            });

            ExecutionReport execReport1_1 = Client1.Receive<ExecutionReport>();
            Assert.Equal(execReport1_1, new ExecutionReport()
            {
                OrderID = guid1_1,
                ExecId = execReport1_1.ExecId,
                ExecTransType = TransactionType.New,
                ExecType = ExecutionType.Filled,
                OrdStatus = OrderStatus.New,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                LeavesQty = 0,
                LastShares = 1000,
                LastPx = 100,
                CumQty = 1000,
                AvgPx = 100
            });

            ExecutionReport execReport1_2 = Client1.Receive<ExecutionReport>();
            Assert.Equal(execReport1_2, new ExecutionReport()
            {
                OrderID = guid1_2,
                ExecId = execReport1_2.ExecId,
                ExecTransType = TransactionType.New,
                ExecType = ExecutionType.Filled,
                OrdStatus = OrderStatus.New,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                LeavesQty = 0,
                LastShares = 1000,
                LastPx = 150,
                CumQty = 1000,
                AvgPx = 150
            });

            Console.WriteLine("exec report 2");
            ExecutionReport execReport2 = Client1.Receive<ExecutionReport>();
            Assert.Equal(execReport2, new ExecutionReport()
            {
                OrderID = guid1_2,
                ExecId = execReport2.ExecId,
                ExecTransType = TransactionType.New,
                ExecType = ExecutionType.Filled,
                OrdStatus = OrderStatus.New,
                Symbol = "Trade-1",
                Side = TradeSide.Buy,
                OrderQty = 2000,
                OrdType = OrderType.Limit,
                LeavesQty = 0,
                LastShares = 1000,
                LastPx = 100,
                CumQty = 2000,
                AvgPx = 125
            });
        }

        // [TestCase("")]
        // public void DoubleNotSamePriceBuyThenSellAll()
        // {
        // }

        // [TestCase("")]
        // public void DoubleSamePriceSellThenBuyAll()
        // {
        // }

        // [TestCase("")]
        // public void DoubleSamePriceBuyThenSellAll()
        // {
        // }

        // [TestCase("")]
        // public void DoubleNotSamePriceSellThenBuyPartial()
        // {
        // }

        // [TestCase("")]
        // public void DoubleNotSamePriceBuyThenSellPartial()
        // {
        // }

        // [TestCase("")]
        // public void DoubleSamePriceSellThenBuyPartial()
        // {
        // }

        // [TestCase("")]
        // public void DoubleSamePriceBuyThenSellPartial()
        // {
        // }
    }
}