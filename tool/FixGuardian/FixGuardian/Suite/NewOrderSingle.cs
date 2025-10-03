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
            Client.AssertNoMoreMessage();
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

    [RequireSuite("NewOrderSingle - Conditional invalid")]
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
            Client.AssertNoMoreMessage();
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

    [RequireSuite("NewOrderSingle - Single user")]
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
            Client1.AssertNoMoreMessage();
            Client2.AssertNoMoreMessage();

            Client1.Logout();
            Client2.Logout();
        }

        /// <summary>
        /// Client 1: Sell 1000 - 100
        /// Client 2: Buy  1000 - 100
        /// </summary>
        [TestCase("Sell then Buy - Same Price")]
        public void SellThenBuySamePrice()
        {
            string guid1 = Guid.NewGuid().ToString();
            Client1.NewOrder(new NewOrderSingle()
            {
                ClOrdId = guid1,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 100
            });

            string guid2 = Guid.NewGuid().ToString();
            Client2.NewOrder(new NewOrderSingle()
            {
                ClOrdId = guid2,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Buy,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 100
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

        /// <summary>
        /// Client 1: Buy  1000 - 100
        /// Client 2: Sell 1000 - 100
        /// </summary>
        [TestCase("Buy then Sell - Same Price")]
        public void BuyThenSell()
        {
            string guid1 = Guid.NewGuid().ToString();
            Client1.NewOrder(new NewOrderSingle()
            {
                ClOrdId = guid1,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Buy,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 100
            });

            string guid2 = Guid.NewGuid().ToString();
            Client2.NewOrder(new NewOrderSingle()
            {
                ClOrdId = guid2,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 100
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

        /// <summary>
        /// Client 1: Sell 1000 - 100
        /// Client 2: Buy  1000 - 200
        /// </summary>
        [TestCase("Sell then Buy - Higher Price")]
        public void SellThenBuyHigher()
        {
            string guid1 = Guid.NewGuid().ToString();
            Client1.NewOrder(new NewOrderSingle()
            {
                ClOrdId = guid1,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 100
            });

            string guid2 = Guid.NewGuid().ToString();
            Client2.NewOrder(new NewOrderSingle()
            {
                ClOrdId = guid2,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Buy,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 200
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

        /// <summary>
        /// Client 1: Buy  1000 - 200
        /// Client 2: Sell 1000 - 100
        /// </summary>
        [TestCase("Buy then Sell - Lower Price")]
        public void BuyThenSellLower()
        {
            string guid1 = Guid.NewGuid().ToString();
            Client1.NewOrder(new NewOrderSingle()
            {
                ClOrdId = guid1,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Buy,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 200
            });

            string guid2 = Guid.NewGuid().ToString();
            Client2.NewOrder(new NewOrderSingle()
            {
                ClOrdId = guid2,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 100
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
                LastPx = 200,
                CumQty = 1000,
                AvgPx = 200
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
                LastPx = 200,
                CumQty = 1000,
                AvgPx = 200
            });
        }

        /// <summary>
        /// Client 1: Sell 1000 - 100
        /// Client 1: Sell 1000 - 150
        /// Client 2: Buy  2000 - 200
        /// </summary>
        [TestCase("Sell then Buy - Double not same - All")]
        public void DoubleNotSamePriceSellThenBuyAll()
        {
            string guid1_1 = Guid.NewGuid().ToString();
            Client1.NewOrder(new NewOrderSingle()
            {
                ClOrdId = guid1_1,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 100
            });

            string guid1_2 = Guid.NewGuid().ToString();
            Client1.NewOrder(new NewOrderSingle()
            {
                ClOrdId = guid1_2,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 150
            });

            string guid2 = Guid.NewGuid().ToString();
            Client2.NewOrder(new NewOrderSingle()
            {
                ClOrdId = guid2,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Buy,
                OrderQty = 2000,
                OrdType = OrderType.Limit,
                Price = 200
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

            ExecutionReport execReport2 = Client2.Receive<ExecutionReport>();
            Assert.Equal(execReport2, new ExecutionReport()
            {
                OrderID = guid2,
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
                LastPx = 150,
                CumQty = 2000,
                AvgPx = 125
            });
        }

        /// <summary>
        /// Client 1: Buy  1000 - 200
        /// Client 1: Buy  1000 - 150
        /// Client 2: Sell 2000 - 100
        /// </summary>
        [TestCase("Buy then Sell - Double not same - All")]
        public void DoubleNotSamePriceBuyThenSellAll()
        {
            string guid1_1 = Guid.NewGuid().ToString();
            Client1.NewOrder(new NewOrderSingle()
            {
                ClOrdId = guid1_1,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Buy,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 200
            });

            string guid1_2 = Guid.NewGuid().ToString();
            Client1.NewOrder(new NewOrderSingle()
            {
                ClOrdId = guid1_2,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Buy,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 150
            });

            string guid2 = Guid.NewGuid().ToString();
            Client2.NewOrder(new NewOrderSingle()
            {
                ClOrdId = guid2,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 2000,
                OrdType = OrderType.Limit,
                Price = 100
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
                Side = TradeSide.Buy,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                LeavesQty = 0,
                LastShares = 1000,
                LastPx = 200,
                CumQty = 1000,
                AvgPx = 200
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
                Side = TradeSide.Buy,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                LeavesQty = 0,
                LastShares = 1000,
                LastPx = 150,
                CumQty = 1000,
                AvgPx = 150
            });

            ExecutionReport execReport2 = Client2.Receive<ExecutionReport>();
            Assert.Equal(execReport2, new ExecutionReport()
            {
                OrderID = guid2,
                ExecId = execReport2.ExecId,
                ExecTransType = TransactionType.New,
                ExecType = ExecutionType.Filled,
                OrdStatus = OrderStatus.New,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 2000,
                OrdType = OrderType.Limit,
                LeavesQty = 0,
                LastShares = 1000,
                LastPx = 150,
                CumQty = 2000,
                AvgPx = 175
            });
        }

        /// <summary>
        /// Client 1: Sell 1000 - 100
        /// Client 1: Sell 1000 - 100
        /// Client 2: Buy  2000 - 200
        /// </summary>
        [TestCase("Sell then Buy - Same - All")]
        public void DoubleSamePriceSellThenBuyAll()
        {
            string guid1_1 = Guid.NewGuid().ToString();
            Client1.NewOrder(new NewOrderSingle()
            {
                ClOrdId = guid1_1,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 100
            });

            string guid1_2 = Guid.NewGuid().ToString();
            Client1.NewOrder(new NewOrderSingle()
            {
                ClOrdId = guid1_2,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 100
            });

            string guid2 = Guid.NewGuid().ToString();
            Client2.NewOrder(new NewOrderSingle()
            {
                ClOrdId = guid2,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Buy,
                OrderQty = 2000,
                OrdType = OrderType.Limit,
                Price = 200
            });

            ExecutionReport execReport2 = Client2.Receive<ExecutionReport>();
            Assert.Equal(execReport2, new ExecutionReport()
            {
                OrderID = guid2,
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
                AvgPx = 100
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
                LastPx = 100,
                CumQty = 1000,
                AvgPx = 100
            });
        }

        /// <summary>
        /// Client 1: Buy  1000 - 200
        /// Client 1: Buy  1000 - 200
        /// Client 2: Sell 2000 - 100
        /// </summary>
        [TestCase("Buy then Sell - Same - All")]
        public void DoubleSamePriceBuyThenSellAll()
        {
            string guid1_1 = Guid.NewGuid().ToString();
            Client1.NewOrder(new NewOrderSingle()
            {
                ClOrdId = guid1_1,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Buy,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 200
            });

            string guid1_2 = Guid.NewGuid().ToString();
            Client1.NewOrder(new NewOrderSingle()
            {
                ClOrdId = guid1_2,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Buy,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 200
            });

            string guid2 = Guid.NewGuid().ToString();
            Client2.NewOrder(new NewOrderSingle()
            {
                ClOrdId = guid2,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 2000,
                OrdType = OrderType.Limit,
                Price = 100
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
                Side = TradeSide.Buy,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                LeavesQty = 0,
                LastShares = 1000,
                LastPx = 200,
                CumQty = 1000,
                AvgPx = 200
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
                Side = TradeSide.Buy,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                LeavesQty = 0,
                LastShares = 1000,
                LastPx = 200,
                CumQty = 1000,
                AvgPx = 200
            });

            ExecutionReport execReport2 = Client2.Receive<ExecutionReport>();
            Assert.Equal(execReport2, new ExecutionReport()
            {
                OrderID = guid2,
                ExecId = execReport2.ExecId,
                ExecTransType = TransactionType.New,
                ExecType = ExecutionType.Filled,
                OrdStatus = OrderStatus.New,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 2000,
                OrdType = OrderType.Limit,
                LeavesQty = 0,
                LastShares = 1000,
                LastPx = 200,
                CumQty = 2000,
                AvgPx = 200
            });
        }

    }

    [RequireSuite("NewOrderSingle - Double user")]
    [RequireSuite("Cancel validation")]
    [TestSuite("NewOrderSingle - Double user - Partial")]
    public class NewOrderSingleDoubleUserPartial
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
            Client1.AssertNoMoreMessage();
            Client2.AssertNoMoreMessage();

            Client1.Logout();
            Client2.Logout();
        }

        /// <summary>
        /// Client 1: Sell 1000 - 100
        /// Client 2: Buy  500 - 150
        /// </summary>
        [TestCase("Sell then Buy")]
        public void SellThenBuyPartial()
        {
            string guid1 = Guid.NewGuid().ToString();
            Client1.NewOrder(new NewOrderSingle()
            {
                ClOrdId = guid1,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 100
            });

            string guid2 = Guid.NewGuid().ToString();
            Client2.NewOrder(new NewOrderSingle()
            {
                ClOrdId = guid2,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Buy,
                OrderQty = 500,
                OrdType = OrderType.Limit,
                Price = 150
            });

            ExecutionReport orderReport1 = Client1.Receive<ExecutionReport>();
            Assert.Equal(orderReport1, new ExecutionReport()
            {
                OrderID = guid1,
                ExecId = orderReport1.ExecId,
                ExecTransType = TransactionType.New,
                ExecType = ExecutionType.PartiallyFilled,
                OrdStatus = OrderStatus.New,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                LeavesQty = 500,
                LastShares = 500,
                LastPx = 100,
                CumQty = 500,
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
                OrderQty = 500,
                OrdType = OrderType.Limit,
                LeavesQty = 0,
                LastShares = 500,
                LastPx = 100,
                CumQty = 500,
                AvgPx = 100
            });

            string cancelGuid = Guid.NewGuid().ToString();
            Client1.Cancel(new OrderCancelRequest()
            {
                OrigClOrdID = guid1,
                ClOrdId = cancelGuid,
                Symbol = "Trade-1",
                OrderQty = 500,
                Side = TradeSide.Sell,
                TransactTime = DateTime.Now,
            });
        }

        /// <summary>
        /// Client 1: Buy  1000 - 200
        /// Client 2: Sell 500 - 150
        /// </summary>
        [TestCase("Buy then Sell")]
        public void BuyThenSellPartial()
        {
            string guid1 = Guid.NewGuid().ToString();
            Client1.NewOrder(new NewOrderSingle()
            {
                ClOrdId = guid1,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Buy,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 200
            });

            string guid2 = Guid.NewGuid().ToString();
            Client2.NewOrder(new NewOrderSingle()
            {
                ClOrdId = guid2,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 500,
                OrdType = OrderType.Limit,
                Price = 150
            });

            ExecutionReport orderReport1 = Client1.Receive<ExecutionReport>();
            Assert.Equal(orderReport1, new ExecutionReport()
            {
                OrderID = guid1,
                ExecId = orderReport1.ExecId,
                ExecTransType = TransactionType.New,
                ExecType = ExecutionType.PartiallyFilled,
                OrdStatus = OrderStatus.New,
                Symbol = "Trade-1",
                Side = TradeSide.Buy,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                LeavesQty = 500,
                LastShares = 500,
                LastPx = 200,
                CumQty = 500,
                AvgPx = 200
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
                OrderQty = 500,
                OrdType = OrderType.Limit,
                LeavesQty = 0,
                LastShares = 500,
                LastPx = 200,
                CumQty = 500,
                AvgPx = 200
            });

            string cancelGuid = Guid.NewGuid().ToString();
            Client1.Cancel(new OrderCancelRequest()
            {
                OrigClOrdID = guid1,
                ClOrdId = cancelGuid,
                Symbol = "Trade-1",
                OrderQty = 500,
                Side = TradeSide.Buy,
                TransactTime = DateTime.Now,
            });
        }

        /// <summary>
        /// Client 1: Sell 1000 - 100
        /// Client 1: Sell 1000 - 100
        /// Client 2: Buy  1500 - 150
        /// </summary>
        [TestCase("Double Sell then Buy - Same Price")]
        public void DoubleSamePriceSellThenBuyPartial()
        {
            string guid1_1 = Guid.NewGuid().ToString();
            Client1.NewOrder(new NewOrderSingle()
            {
                ClOrdId = guid1_1,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 100
            });

            string guid1_2 = Guid.NewGuid().ToString();
            Client1.NewOrder(new NewOrderSingle()
            {
                ClOrdId = guid1_2,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 100
            });

            string guid2 = Guid.NewGuid().ToString();
            Client2.NewOrder(new NewOrderSingle()
            {
                ClOrdId = guid2,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Buy,
                OrderQty = 1500,
                OrdType = OrderType.Limit,
                Price = 150
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
                ExecType = ExecutionType.PartiallyFilled,
                OrdStatus = OrderStatus.New,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                LeavesQty = 500,
                LastShares = 500,
                LastPx = 100,
                CumQty = 500,
                AvgPx = 100
            });

            ExecutionReport execReport2 = Client2.Receive<ExecutionReport>();
            Assert.Equal(execReport2, new ExecutionReport()
            {
                OrderID = guid2,
                ExecId = execReport2.ExecId,
                ExecTransType = TransactionType.New,
                ExecType = ExecutionType.Filled,
                OrdStatus = OrderStatus.New,
                Symbol = "Trade-1",
                Side = TradeSide.Buy,
                OrderQty = 1500,
                OrdType = OrderType.Limit,
                LeavesQty = 0,
                LastShares = 500,
                LastPx = 100,
                CumQty = 1500,
                AvgPx = 100
            });

            string cancelGuid = Guid.NewGuid().ToString();
            Client1.Cancel(new OrderCancelRequest()
            {
                OrigClOrdID = guid1_2,
                ClOrdId = cancelGuid,
                Symbol = "Trade-1",
                OrderQty = 500,
                Side = TradeSide.Sell,
                TransactTime = DateTime.Now,
            });
        }

        /// <summary>
        /// Client 1: Buy  1000 - 150
        /// Client 1: Buy  1000 - 150
        /// Client 2: Sell 1500 - 100
        /// </summary>
        [TestCase("Double Buy then Sell - Same Price")]
        public void DoubleSamePriceBuyThenSellPartial()
        {
            string guid1_1 = Guid.NewGuid().ToString();
            Client1.NewOrder(new NewOrderSingle()
            {
                ClOrdId = guid1_1,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Buy,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 150
            });

            string guid1_2 = Guid.NewGuid().ToString();
            Client1.NewOrder(new NewOrderSingle()
            {
                ClOrdId = guid1_2,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Buy,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 150
            });

            string guid2 = Guid.NewGuid().ToString();
            Client2.NewOrder(new NewOrderSingle()
            {
                ClOrdId = guid2,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 1500,
                OrdType = OrderType.Limit,
                Price = 100
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
                Side = TradeSide.Buy,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                LeavesQty = 0,
                LastShares = 1000,
                LastPx = 150,
                CumQty = 1000,
                AvgPx = 150
            });

            ExecutionReport execReport1_2 = Client1.Receive<ExecutionReport>();
            Assert.Equal(execReport1_2, new ExecutionReport()
            {
                OrderID = guid1_2,
                ExecId = execReport1_2.ExecId,
                ExecTransType = TransactionType.New,
                ExecType = ExecutionType.PartiallyFilled,
                OrdStatus = OrderStatus.New,
                Symbol = "Trade-1",
                Side = TradeSide.Buy,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                LeavesQty = 500,
                LastShares = 500,
                LastPx = 150,
                CumQty = 500,
                AvgPx = 150
            });

            ExecutionReport execReport2 = Client2.Receive<ExecutionReport>();
            Assert.Equal(execReport2, new ExecutionReport()
            {
                OrderID = guid2,
                ExecId = execReport2.ExecId,
                ExecTransType = TransactionType.New,
                ExecType = ExecutionType.Filled,
                OrdStatus = OrderStatus.New,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 1500,
                OrdType = OrderType.Limit,
                LeavesQty = 0,
                LastShares = 500,
                LastPx = 150,
                CumQty = 1500,
                AvgPx = 150
            });

            string cancelGuid = Guid.NewGuid().ToString();
            Client1.Cancel(new OrderCancelRequest()
            {
                OrigClOrdID = guid1_2,
                ClOrdId = cancelGuid,
                Symbol = "Trade-1",
                OrderQty = 500,
                Side = TradeSide.Buy,
                TransactTime = DateTime.Now,
            });
        }


        /// <summary>
        /// Client 1: Sell 1000 - 100
        /// Client 1: Sell 1000 - 150
        /// Client 2: Buy  1500 - 200
        /// </summary>
        [TestCase("Double Sell then Buy - Not same Price")]
        public void DoubleNotSamePriceSellThenBuyPartial()
        {
            string guid1_1 = Guid.NewGuid().ToString();
            Client1.NewOrder(new NewOrderSingle()
            {
                ClOrdId = guid1_1,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 100
            });

            string guid1_2 = Guid.NewGuid().ToString();
            Client1.NewOrder(new NewOrderSingle()
            {
                ClOrdId = guid1_2,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 150
            });

            string guid2 = Guid.NewGuid().ToString();
            Client2.NewOrder(new NewOrderSingle()
            {
                ClOrdId = guid2,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Buy,
                OrderQty = 1500,
                OrdType = OrderType.Limit,
                Price = 200
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
                ExecType = ExecutionType.PartiallyFilled,
                OrdStatus = OrderStatus.New,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                LeavesQty = 500,
                LastShares = 500,
                LastPx = 150,
                CumQty = 500,
                AvgPx = 150
            });

            ExecutionReport execReport2 = Client2.Receive<ExecutionReport>();
            Assert.Equal(execReport2, new ExecutionReport()
            {
                OrderID = guid2,
                ExecId = execReport2.ExecId,
                ExecTransType = TransactionType.New,
                ExecType = ExecutionType.Filled,
                OrdStatus = OrderStatus.New,
                Symbol = "Trade-1",
                Side = TradeSide.Buy,
                OrderQty = 1500,
                OrdType = OrderType.Limit,
                LeavesQty = 0,
                LastShares = 500,
                LastPx = 150,
                CumQty = 1500,
                AvgPx = (1000 * 100 + 500 * 150) / (float)1500
            });

            string cancelGuid = Guid.NewGuid().ToString();
            Client1.Cancel(new OrderCancelRequest()
            {
                OrigClOrdID = guid1_2,
                ClOrdId = cancelGuid,
                Symbol = "Trade-1",
                OrderQty = 500,
                Side = TradeSide.Sell,
                TransactTime = DateTime.Now,
            });
        }

        /// <summary>
        /// Client 1: Buy  1000 - 200
        /// Client 1: Buy  1000 - 150
        /// Client 2: Sell 1500 - 100
        /// </summary>
        [TestCase("Double Buy then Sell - Not same Price")]
        public void DoubleNotSamePriceBuyThenSellPartial()
        {
            string guid1_1 = Guid.NewGuid().ToString();
            Client1.NewOrder(new NewOrderSingle()
            {
                ClOrdId = guid1_1,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Buy,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 200
            });

            string guid1_2 = Guid.NewGuid().ToString();
            Client1.NewOrder(new NewOrderSingle()
            {
                ClOrdId = guid1_2,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Buy,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 150
            });

            string guid2 = Guid.NewGuid().ToString();
            Client2.NewOrder(new NewOrderSingle()
            {
                ClOrdId = guid2,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 1500,
                OrdType = OrderType.Limit,
                Price = 100
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
                Side = TradeSide.Buy,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                LeavesQty = 0,
                LastShares = 1000,
                LastPx = 200,
                CumQty = 1000,
                AvgPx = 200
            });

            ExecutionReport execReport1_2 = Client1.Receive<ExecutionReport>();
            Assert.Equal(execReport1_2, new ExecutionReport()
            {
                OrderID = guid1_2,
                ExecId = execReport1_2.ExecId,
                ExecTransType = TransactionType.New,
                ExecType = ExecutionType.PartiallyFilled,
                OrdStatus = OrderStatus.New,
                Symbol = "Trade-1",
                Side = TradeSide.Buy,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                LeavesQty = 500,
                LastShares = 500,
                LastPx = 150,
                CumQty = 500,
                AvgPx = 150
            });

            ExecutionReport execReport2 = Client2.Receive<ExecutionReport>();
            Assert.Equal(execReport2, new ExecutionReport()
            {
                OrderID = guid2,
                ExecId = execReport2.ExecId,
                ExecTransType = TransactionType.New,
                ExecType = ExecutionType.Filled,
                OrdStatus = OrderStatus.New,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 1500,
                OrdType = OrderType.Limit,
                LeavesQty = 0,
                LastShares = 500,
                LastPx = 150,
                CumQty = 1500,
                AvgPx = (1000 * 200 + 500 * 150) / (float)1500
            });

            string cancelGuid = Guid.NewGuid().ToString();
            Client1.Cancel(new OrderCancelRequest()
            {
                OrigClOrdID = guid1_2,
                ClOrdId = cancelGuid,
                Symbol = "Trade-1",
                OrderQty = 500,
                Side = TradeSide.Buy,
                TransactTime = DateTime.Now,
            });
        }

        /// <summary>
        /// Client 1: Buy  500 - 150
        /// Client 2: Sell 1000 - 100
        /// Client 1: Buy  500 - 200
        /// </summary>
        [TestCase("Buy - Sell - Buy")]
        public void BuySellSell()
        {
            string guid1 = Guid.NewGuid().ToString();
            Client1.NewOrder(new NewOrderSingle()
            {
                ClOrdId = guid1,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Buy,
                OrderQty = 500,
                OrdType = OrderType.Limit,
                Price = 150
            });

            string guid2 = Guid.NewGuid().ToString();
            Client2.NewOrder(new NewOrderSingle()
            {
                ClOrdId = guid2,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                Price = 100
            });

            ExecutionReport execReport1_1 = Client1.Receive<ExecutionReport>();
            Assert.Equal(execReport1_1, new ExecutionReport()
            {
                OrderID = guid1,
                ExecId = execReport1_1.ExecId,
                ExecTransType = TransactionType.New,
                ExecType = ExecutionType.Filled,
                OrdStatus = OrderStatus.New,
                Symbol = "Trade-1",
                Side = TradeSide.Buy,
                OrderQty = 500,
                OrdType = OrderType.Limit,
                LeavesQty = 0,
                LastShares = 500,
                LastPx = 150,
                CumQty = 500,
                AvgPx = 150
            });

            Console.WriteLine("exec report 2:");

            ExecutionReport execReport2_1 = Client2.Receive<ExecutionReport>();
            Assert.Equal(execReport2_1, new ExecutionReport()
            {
                OrderID = guid2,
                ExecId = execReport2_1.ExecId,
                ExecTransType = TransactionType.New,
                ExecType = ExecutionType.PartiallyFilled,
                OrdStatus = OrderStatus.New,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                LeavesQty = 500,
                LastShares = 500,
                LastPx = 150,
                CumQty = 500,
                AvgPx = 150
            });

            Console.WriteLine("First wave done");

            string guid3 = Guid.NewGuid().ToString();
            Client1.NewOrder(new NewOrderSingle()
            {
                ClOrdId = guid3,
                HandlInst = HandleInstance.Manual,
                Symbol = "Trade-1",
                Side = TradeSide.Buy,
                OrderQty = 500,
                OrdType = OrderType.Limit,
                Price = 200
            });

            ExecutionReport execReport1_2 = Client1.Receive<ExecutionReport>();
            Assert.Equal(execReport1_2, new ExecutionReport()
            {
                OrderID = guid3,
                ExecId = execReport1_2.ExecId,
                ExecTransType = TransactionType.New,
                ExecType = ExecutionType.Filled,
                OrdStatus = OrderStatus.New,
                Symbol = "Trade-1",
                Side = TradeSide.Buy,
                OrderQty = 500,
                OrdType = OrderType.Limit,
                LeavesQty = 0,
                LastShares = 500,
                LastPx = 100,
                CumQty = 500,
                AvgPx = 100
            });

            ExecutionReport execReport2_2 = Client2.Receive<ExecutionReport>();
            Assert.Equal(execReport2_2, new ExecutionReport()
            {
                OrderID = guid2,
                ExecId = execReport2_2.ExecId,
                ExecTransType = TransactionType.New,
                ExecType = ExecutionType.Filled,
                OrdStatus = OrderStatus.PartiallyFilled,
                Symbol = "Trade-1",
                Side = TradeSide.Sell,
                OrderQty = 1000,
                OrdType = OrderType.Limit,
                LeavesQty = 0,
                LastShares = 500,
                LastPx = 100,
                CumQty = 1000,
                AvgPx = (500 * 150 + 500 * 100) / (float)1000
            });
        }
    }
}