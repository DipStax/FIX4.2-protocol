#include <numeric>

#include "Server/Core/OrderBook.hpp"
#include "Common/Message/ExecutionReport.hpp"
#include "Server/Core/Pipeline/Naming.hpp"

OrderBook::OrderBook(const std::string &_name, EventQueue &_output)
    : m_name(_name), m_output(_output)
{
}

bool OrderBook::add(OrderType _type, Price _price, Order& _order)
{
    if (has(_type, _order.orderId))
        return false;
    add(_type, _price, _order, OrderStatus::New);
    return true;
}

bool OrderBook::modify(OrderType _type, Price _price, Order &_order)
{
    if (has(_type, _order.orderId))
        return false;
    add(_type, _price, _order, OrderStatus::Replaced);
    return true;
}

bool OrderBook::cancel(OrderType _type, OrderId _orderId, bool _notif)
{
    if (_type == OrderType::Ask)
        return cancel<AskBook>(m_ask_id, _orderId, _notif);
    return cancel<BidBook>(m_bid_id, _orderId, _notif);
}

bool OrderBook::has(OrderType _type, OrderId _orderId) const
{
    if (_type == OrderType::Ask)
        return m_ask_id.contains(_orderId);
    return m_bid_id.contains(_orderId);
}

fix::MarketDataSnapshotFullRefresh OrderBook::refresh(const OrderBook::Subscription &_sub)
{
    if (m_is_cached) {
        if (_sub.type == OrderType::Bid)
            return refresh<cache_BidBook>(m_cache_bid_full, _sub.depth);
        return refresh<cache_AskBook>(m_cache_ask_full, _sub.depth);
    }
    cache_on<AskBook, cache_AskBook>(m_ask, m_cache_ask_full);
    Logger::Log("[OrderBook] {FullRefresh} Cached ask size: ", m_cache_bid.size());
    cache_on<BidBook, cache_BidBook>(m_bid, m_cache_bid_full);
    Logger::Log("[OrderBook] {FullRefresh} Cached bid size: ", m_cache_bid.size());
    m_is_cached = true;
    return refresh(_sub);
}

fix::MarketDataIncrementalRefresh OrderBook::update(const OrderBook::Subscription &_sub)
{
    if (m_is_cached && m_is_cached_udp) {
        if (_sub.type == OrderType::Bid)
            return update<cache_BidBook>(m_cache_bid, m_cache_bid_upd, _sub.depth);
        return update<cache_AskBook>(m_cache_ask, m_cache_ask_upd, _sub.depth);
    }
    m_cache_ask_upd = m_cache_ask;
    Logger::Log("[OrderBook] {FullRefresh} Cached old bid size: ", m_cache_ask_upd.size());
    m_cache_bid_upd = m_cache_bid;
    Logger::Log("[OrderBook] {FullRefresh} Cached old bid size: ", m_cache_bid_upd.size());
    m_is_cached_udp = true;

    std::lock_guard<std::mutex> guard(m_mutex);

    cache_on<AskBook, cache_AskBook>(m_ask, m_cache_ask);
    Logger::Log("[OrderBook] {FullRefresh} Cached new ask size: ", m_cache_ask.size());
    cache_on<BidBook, cache_BidBook>(m_bid, m_cache_bid);
    Logger::Log("[OrderBook] {FullRefresh} Cached new bid size: ", m_cache_bid.size());
    m_is_cached = true;
    return update(_sub);
}

void OrderBook::cache_flush()
{
    m_is_cached = false;
    m_is_cached_udp = false;
}

bool OrderBook::contain(OrderType _type, Price _price)
{
    std::lock_guard<std::mutex> guard(m_mutex);

    if (_type == OrderType::Ask)
        return m_ask.contains(_price);
    return m_bid.contains(_price);
}

void OrderBook::add(OrderType _type, Price _price, Order &_order, OrderStatus _status)
{
    Event event;

    event.quantity = 0;
    event.orderId = _order.orderId;
    event.orgQty = _order.quantity;
    event.userId = _order.userId;
    event.price = _price;
    event.side = _type;
    event.sold = false;
    if (_type == OrderType::Bid) {
        if (add<AskBook, std::greater_equal<Price>>(m_ask, _price, _order)) {
            std::lock_guard<std::mutex> guard(m_mutex);

            Logger::Log("[OrderBook] (", m_name, ") {Add} New order in BID: ", _order, " at price: ", _price);
            m_bid[_price].push_back(_order);
            m_bid_id.emplace(_order.orderId, std::make_pair(m_bid.find(_price), m_bid.at(_price).end() - 1));
            event.status = OrderStatus::PartiallyFilled;
        }
    } else {
        if (add<BidBook, std::less_equal<Price>>(m_bid, _price, _order)) {
            std::lock_guard<std::mutex> guard(m_mutex);

            Logger::Log("[OrderBook] (", m_name, ") {Add} New order in ASK: ", _order, " at price: ", _price);
            m_ask[_price].push_back(_order);
            m_ask_id.emplace(_order.orderId, std::make_pair(m_ask.find(_price), m_ask.at(_price).end() - 1));
            event.status = OrderStatus::PartiallyFilled;
        }
    }
    event.quantity = _order.quantity;
    if (event.quantity == event.orgQty)
        event.status = _status;
    else if (event.quantity != 0)
        event.status = OrderStatus::PartiallyFilled;
    else
        event.status = OrderStatus::Filled;
    Logger::Log("[OrderBook] (", m_name, ") {Add} New order event: "); // todo log
    m_output.append(event);
    Logger::Log("[OrderBook] (", m_name, ") {Add} New order event send");
}