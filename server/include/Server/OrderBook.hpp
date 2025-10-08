#pragma once

#include <list>
#include <concepts>
#include <shared_mutex>
#include <map>
#include <unordered_map>
#include <optional>

#include "Server/meta.hpp"

#include "Shared/Core/Core.hpp"
#include "Shared/Thread/Queue.hpp"
#include "Shared/Log/ILogger.hpp"
#include "Shared/Core/Enum.hpp"

class OrderBook
{
    public:
        struct OrderInfo
        {
            std::string execid;
            Price price;
            Order order;
        };

        struct Event
        {
            fix42::ExecutionStatus execStatus;
            Price lastPrice;
            Quantity lastQty;
            Order order;
        };

        struct IdList
        {
            struct Info
            {
                OrderList::iterator Order;
                Price price;
            };

            using OrderIdMap = std::unordered_map<OrderId, Info>;

            std::shared_mutex mutex;
            OrderIdMap list;
        };

        template<class T>
        struct OrderBookSide
        {
            struct PriceOrderList
            {
                Quantity cumQty;

                std::shared_mutex mutex;
                OrderList list;
            };

            using BookType = std::map<Price, PriceOrderList, T>;

            IdList idlist;

            std::shared_mutex mutex;
            BookType book;
        };

        // using AskBook = std::map<Price, OrderListBundle, std::less<Price>>;
        // using BidBook = std::map<Price, OrderListBundle, std::greater<Price>>;

        OrderBook(const std::string &_name, ts::Queue<Event> &_event);
        virtual ~OrderBook() = default;

        [[nodiscard]] const std::string &getSymbol() const;

        [[nodiscard]] bool allowTick(fix42::Side _side);
        [[nodiscard]] bool has(const OrderId &_orderId);
        [[nodiscard]] bool has(const OrderId &_orderId, fix42::Side _side);

        std::optional<Order> getOrder(const OrderId &_orderId);

        void add(const OrderInfo &_order);
        bool cancel(const OrderId &_orderId, fix42::Side _side);

        void lockReadOrderId(fix42::Side _side);
        void unlockReadOrderId(fix42::Side _side);

    private:
        enum FillStatus
        {
            Filled,
            PartialyFilled,
            Skipped
        };

        [[nodiscard]] bool has(IdList &_idlist, const OrderId &_order);

        [[nodiscard]] std::optional<IdList::Info> getOrderIdInfo(const OrderId &_orderId, fix42::Side _side);
        [[nodiscard]] std::optional<IdList::Info> getOrderIdInfo(IdList &, const OrderId &_orderId);

        template<class FillSide, class OrderSide>
        void add(FillSide &_fillside, OrderSide &_orderside, const OrderInfo &_order);


        /// @return The remaining quantity of the order and it's average price.
        template<class Cmp>
        std::pair<Quantity, Price> fillOnBook(OrderBookSide<Cmp> &_book_side, const OrderInfo &_order);
        FillStatus fillOrder(Event &_main_event, Price _price, Order &_order);

        template<class Cmp>
        void addToBook(OrderBookSide<Cmp> &_book, Price _price, const Order &_order);

        template<class Cmp>
        void cancelOrder(OrderBookSide<Cmp> &_book, const IdList::Info &_info);

        bool removeFromIdMap(IdList &_idlist, const OrderId &_orderid);

        void computeTick(Price _new);
        void computeTick();

        const std::string m_name;

        Price m_lastprice = 0.f;
        Price m_lastprice_diff = 0.f;
        fix42::TickDir m_tick = fix42::TickDir::ZeroPlusTick;

        ts::Queue<Event> &m_event_output;

        OrderBookSide<std::less<Price>> m_ask_side;
        OrderBookSide<std::greater<Price>> m_bid_side;

        std::unique_ptr<logger::ILogger> Logger = nullptr;
};

#include "Server/OrderBook.inl"