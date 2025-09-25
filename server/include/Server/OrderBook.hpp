#pragma once

#include <list>
#include <concepts>
#include <shared_mutex>
#include <map>
#include <unordered_map>

#include "Server/meta.hpp"

#include "Shared/Core/Core.hpp"
#include "Shared/Thread/Queue.hpp"
#include "Shared/Log/ILogger.hpp"
#include "Shared/Core/Enum.hpp"

template<class T>
concept IsBook = IsBookOf<T, OrderList>;

class OrderBook
{
    public:
        struct OrderInfo
        {
            std::string execid;
            Price price;
            Order order;
        };

        struct OrderIdInfo
        {
            OrderList::iterator Order;
            Price price;
        };

        struct Event
        {
            fix42::ExecutionStatus execStatus;
            Price lastPrice;
            Quantity lastQty;
            Order order;
        };

        enum TickDirection
        {
            PlusTick,
            ZeroPlusTick,
            MinusTick,
            ZeroMinusTick
        };

        using AskBook = std::map<Price, OrderList, std::greater<Price>>;
        using BidBook = std::map<Price, OrderList, std::less<Price>>;
        using OrderIdMap = std::unordered_map<OrderId, OrderIdInfo>;

        OrderBook(const std::string &_name, ts::Queue<Event> &_event);
        virtual ~OrderBook() = default;

        [[nodiscard]] const std::string &getSymbol() const;

        [[nodiscard]] bool allowTick(fix42::Side _side);
        [[nodiscard]] bool has(const OrderId &_orderId);
        [[nodiscard]] bool has(const OrderId &_orderId, fix42::Side _side);

        Order getOrder(const OrderId &_orderId);

        bool add(const OrderInfo &_order);

    protected:
        struct OrderIdMapBundle
        {
            std::shared_mutex Mutex{};
            OrderIdMap IdList{};
        };

        enum FillStatus
        {
            Filled,
            PartialyFilled,
            Skipped
        };

        /// @brief 
        /// @tparam Comparator 
        /// @tparam BookType 
        /// @param _book 
        /// @param _idmap 
        /// @param _order 
        /// @return The remaining quantity of the order.
        template<class Comparator, IsBook BookType>
        Quantity fillOnBook(BookType &_book, OrderIdMapBundle &_idmap, const OrderInfo &_order);
        FillStatus fillOrder(Event &_main_event, Price _price, Order &_order);

        template<IsBook BookType>
        void addToBook(BookType &_book, OrderIdMapBundle &_idmap, Price _price, const Order &_order);

        bool removeFromIdMap(OrderIdMapBundle &_idmap, const OrderId &_orderid);

    private:
        void computeTick(Price _new);
        void computeTick();

        const std::string m_name;

        Price m_lastprice = 0.f;
        Price m_lastprice_diff = 0.f;
        TickDirection m_tick = TickDirection::ZeroPlusTick;

        ts::Queue<Event> &m_event_output;

        OrderIdMapBundle m_bid_id;
        OrderIdMapBundle m_ask_id;

        AskBook m_ask_book;
        BidBook m_bid_book;

        std::unique_ptr<logger::ILogger> Logger = nullptr;
};

#include "Server/OrderBook.inl"