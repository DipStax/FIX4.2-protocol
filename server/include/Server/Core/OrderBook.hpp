#pragma once

#include <mutex>
#include <map>
#include <unordered_map>

#include "Common/Core/Order.hpp"
#include "Server/Core/meta.hpp"
#include "Common/Thread/Queue.hpp"
#include "Common/Message/MarketDataSnapshotFullRefresh.hpp"
#include "Common/Message/MarketDataIncrementalRefresh.hpp"

using AskBook = std::map<Price, OrderList, std::greater<Price>>;
using BidBook = std::map<Price, OrderList, std::less<Price>>;

class OrderBook
{
    public:
        struct Event
        {
            OrderId orderId;
            Quantity orgQty;
            OrderStatus status;
            Price price;
            Quantity quantity;
            OrderType side;
            UserId userId;
            bool sold;
        };

        struct Data
        {
            enum class Action
            {
                Add,
                Modify,
                Cancel
            };

            Action action;
            OrderType type;
            Price price;
            Price oprice;
            OrderId target;
            Order order;
        };

        struct Subscription
        {
            size_t depth = 0;
            OrderType type;
            std::string Id = "";
        };

        using EventQueue = ts::Queue<Event>;

        OrderBook(const std::string &_name, EventQueue &_output);
        virtual ~OrderBook() = default;

        [[nodiscard]] bool add(OrderType _type, Price _price, Order &_order);
        [[nodiscard]] bool modify(OrderType _type, Price _price, Order &_order);
        [[nodiscard]] bool cancel(OrderType _type, OrderId _orderId, bool _event = true);

        [[nodiscard]] bool has(OrderType _type, OrderId _orderId) const;

        [[nodiscard]] fix::MarketDataSnapshotFullRefresh refresh(const OrderBook::Subscription &_sub);
        [[nodiscard]] fix::MarketDataIncrementalRefresh update(const OrderBook::Subscription &_sub);

        void cache_flush();

        [[nodiscard]] bool contain(OrderType _type, Price _price);

    protected:
        template<IsBook T>
        using OrderIdMap = std::unordered_map<OrderId, std::pair<typename T::iterator, OrderList::iterator>>;

        void add(OrderType _type, Price _price, Order &_order, OrderStatus _status);

        template<IsBook T, class _T>
        bool add(T &_book, Price _price, Order &_order);
        template<IsBook T>
        bool cancel(OrderIdMap<T> &_mapId, OrderId _orderId, bool _event);

        template<IsBookCache T>
        fix::MarketDataSnapshotFullRefresh refresh(T &_cache, size_t _depth);
        template<IsBookCache T>
        fix::MarketDataIncrementalRefresh update(T &_cache_orig, T &_cache, size_t _depth);

    private:
        using cache_AskBook = std::map<Price, Quantity, std::greater_equal<Price>>;
        using cache_BidBook = std::map<Price, Quantity, std::less_equal<Price>>;

        template<IsBook T, IsBookCache _T>
        void cache_on(T &_book, _T &_cache);

        bool m_is_cached = false;
        bool m_is_cached_udp = false;

        cache_AskBook m_cache_ask{};
        cache_BidBook m_cache_bid{};

        cache_AskBook m_cache_ask_upd{};
        cache_BidBook m_cache_bid_upd{};

        cache_AskBook m_cache_ask_full{};
        cache_BidBook m_cache_bid_full{};

        const std::string m_name;

        std::mutex m_mutex;

        OrderIdMap<BidBook> m_bid_id{};
        OrderIdMap<AskBook> m_ask_id{};

        EventQueue &m_output;

        BidBook m_bid{};
        AskBook m_ask{};
};

#include "Server/Core/OrderBook.inl"