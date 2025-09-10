#pragma once

#include <list>
#include <concepts>
#include <shared_mutex>
#include <map>
#include <unordered_map>

#include "Server/meta.hpp"

#include "Shared/Core/Order.hpp"
#include "Shared/Thread/Queue.hpp"
#include "Shared/Log/ILogger.hpp"

/**
 * @namespace obs
 * @brief Order Book System namespace containing order management structures
 */
namespace obs
{
    /**
     * @struct OrderListBundle
     * @brief Thread-safe container for order lists at a specific price level
     * 
     * Combines a list of orders with a shared mutex for concurrent access.
     * Uses shared_mutex to allow multiple readers but exclusive writers.
     */
    struct OrderListBundle
    {
        std::shared_mutex Mutex;  ///< Mutex for thread-safe access (multiple readers, single writer)
        OrderList List;            ///< List of orders at this price level
    };

    /**
     * @struct OrderInfo
     * @brief Complete information for a new order to be added to the book
     */
    struct OrderInfo
    {
        OrderType side;  ///< Buy (Bid) or Sell (Ask) side
        Price price;     ///< Limit price for the order
        Order order;     ///< Order details (ID, user, quantity)
    };

    /**
     * @struct OrderIdInfo
     * @brief Tracking information for locating an order in the book
     * 
     * Stores iterator to the order in its price level list
     * and the price level itself for efficient order lookup and removal.
     */
    struct OrderIdInfo
    {
        OrderList::iterator Order;  ///< Iterator pointing to order in the list
        Price price;                ///< Price level where order is located
    };

    /**
     * @struct Event
     * @brief Order execution event for reporting trades and order updates
     * 
     * Contains all information needed to generate FIX ExecutionReport messages
     * when orders are placed, matched, partially filled, or fully executed.
     */
    struct Event
    {
        OrderType side;              ///< Side of the order (Bid/Ask)
        OrderId orderId;             ///< Unique order identifier
        UserId userId;               ///< User who placed the order
        Price price;                 ///< Execution price
        Price avgPrice;              ///< Average price for partial fills
        Quantity remainQty;          ///< Remaining quantity after execution
        Quantity orgQty;             ///< Original order quantity
        OrderStatusValue ordStatus;  ///< Current order status
        ExecTypeValue execStatus;    ///< Type of execution event
    };
}

/**
 * @concept IsBook
 * @brief Concept to ensure type is a valid order book (map of price to OrderListBundle)
 */
template<class T>
concept IsBook = IsBookOf<T, obs::OrderListBundle>;

/**
 * @class OrderBook
 * @brief Manages limit order book for a single trading symbol
 * 
 * Implements a price-time priority order matching engine:
 * - Maintains separate bid and ask books sorted by price
 * - Matches incoming orders against opposite side of the book
 * - Supports partial fills and order placement
 * - Thread-safe for concurrent access
 * - Generates execution events for downstream processing
 * 
 * @note Currently only supports limit orders, market orders not implemented
 */
class OrderBook
{
    public:
        /// Ask book type: sorted by price descending (best ask = lowest price first)
        using AskBook = std::map<Price, obs::OrderListBundle, std::greater<Price>>;
        
        /// Bid book type: sorted by price ascending (best bid = highest price first)
        using BidBook = std::map<Price, obs::OrderListBundle, std::less<Price>>;
        
        /// Fast lookup map from order ID to its location in the book
        using OrderIdMap = std::unordered_map<OrderId, obs::OrderIdInfo>;

        /**
         * @brief Constructs an order book for a specific symbol
         * @param _name Trading symbol this book manages
         * @param _event Output queue for execution events
         */
        OrderBook(const std::string &_name, ts::Queue<obs::Event> &_event);
        virtual ~OrderBook() = default;

        /**
         * @brief Adds a new order to the book, matching if possible
         * @param _order Order information including side, price, and quantity
         * @return true if order was processed successfully
         * 
         * Process:
         * 1. Attempts to match against opposite side of book
         * 2. Places any remaining quantity in the book
         * 3. Generates execution events for matches
         */
        bool add(const obs::OrderInfo &_order);

        /**
         * @brief Checks if an order exists in the book
         * @param _orderId Order ID to search for
         * @return true if order is currently in the book
         */
        [[nodiscard]] bool has(const OrderId &_orderId);

        /**
         * @brief Gets the trading symbol this book manages
         * @return Symbol name
         */
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

        /**
         * @brief Matches an incoming order against the opposite side of the book
         * @tparam Comparator Price comparison logic for matching (less_equal for bids, greater_equal for asks)
         * @tparam BookType Type of order book (BidBook or AskBook)
         * @param _book The order book to match against
         * @param _idmap ID mapping for the book being matched against
         * @param _order Incoming order to match
         * @return Remaining quantity after all possible matches
         * 
         * Implements price-time priority matching:
         * - Matches at best available prices first
         * - Within same price, matches oldest orders first (FIFO)
         * - Generates execution events for each match
         */
        template<class Comparator, IsBook BookType>
        Quantity fillOnBook(BookBundle<BookType> &_book, OrderIdMapBundle &_idmap, const obs::OrderInfo &_order);

        /**
         * @brief Adds an order to the specified book at the given price level
         * @tparam BookType Type of order book (BidBook or AskBook)
         * @param _book Book to add order to
         * @param _idmap ID mapping to update
         * @param _price Price level for the order
         * @param _order Order to add
         * 
         * Thread-safe insertion maintaining price-time priority
         */
        template<IsBook BookType>
        void addToBook(BookBundle<BookType> &_book, OrderIdMapBundle &_idmap, Price _price, const Order &_order);

        /**
         * @brief Removes an order from the ID mapping
         * @param _idmap ID map to remove from
         * @param _orderid Order ID to remove
         * @return true if order was found and removed
         */
        bool removeFromIdMap(OrderIdMapBundle &_idmap, const OrderId &_orderid);

    private:
        /// Trading symbol this book manages
        const std::string m_name;

        /// Output queue for execution events (trades, fills, etc.)
        ts::Queue<obs::Event> &m_event_output;

        /// Fast lookup map for bid orders
        OrderIdMapBundle m_bid_id;
        
        /// Fast lookup map for ask orders
        OrderIdMapBundle m_ask_id;

        /// Ask side order book (sell orders)
        BookBundle<AskBook> m_ask_book;
        
        /// Bid side order book (buy orders)
        BookBundle<BidBook> m_bid_book;

        /// Logger for debugging and monitoring
        std::unique_ptr<logger::ILogger> Logger = nullptr;
};

#include "Server/OrderBook.inl"