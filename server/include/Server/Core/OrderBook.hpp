#pragma once

#include <list>
#include <concepts>
#include <shared_mutex>
#include <map>
#include <unordered_map>

#include "Server/Core/meta.hpp"

#include "Common/Core/Order.hpp"
#include "Common/Thread/Queue.hpp"
#include "Common/Log/ILogger.hpp"


namespace obs
{
    struct OrderListBundle
    {
        std::shared_mutex Mutex;
        OrderList List;
    };

    struct OrderInfo
    {
        OrderType side;
        Price price;
        Order order;
    };

    struct OrderIdInfo
    {
        OrderList::iterator Order;
        Price price;
    };
}

template<class T>
concept IsBook = IsBookOf<T, obs::OrderListBundle>;

class OrderBook
{
    public:
        using AskBook = std::map<Price, obs::OrderListBundle, std::greater<Price>>;
        using BidBook = std::map<Price, obs::OrderListBundle, std::less<Price>>;
        using OrderIdMap = std::unordered_map<OrderId, obs::OrderIdInfo>;

        OrderBook(const std::string &_name);
        virtual ~OrderBook() = default;

        [[nodiscard]] bool add(const obs::OrderInfo &_order);

        [[nodiscard]] bool has(const OrderId &_orderId);

        [[nodiscard]] const std::string &getSymbol() const;

    protected:
        struct OrderIdMapBundle
        {
            std::shared_mutex Mutex{};
            OrderIdMap IdList{};
        };

        template<IsBook BookType>
        struct BookBundle
        {
            std::shared_mutex Mutex{};
            BookType Book{};
        };

        /// @brief 
        /// @tparam Comparator 
        /// @tparam BookType 
        /// @param _book 
        /// @param _idmap 
        /// @param _order 
        /// @return The remaining quantity of the order.
        template<class Comparator, IsBook BookType>
        Quantity fillOnBook(BookBundle<BookType> &_book, OrderIdMapBundle &_idmap, const obs::OrderInfo &_order);

        template<IsBook BookType>
        void addToBook(BookBundle<BookType> &_book, OrderIdMapBundle &_idmap, Price _price, const Order &_order);

        bool removeFromIdMap(OrderIdMapBundle &_idmap, const OrderId &_orderid);

    private:
        const std::string m_name;

        OrderIdMapBundle m_bid_id;
        OrderIdMapBundle m_ask_id;

        BookBundle<AskBook> m_ask_book;
        BookBundle<BidBook> m_bid_book;

        std::unique_ptr<logger::ILogger> Logger = nullptr;
};

#include "Server/Core/OrderBook.inl"