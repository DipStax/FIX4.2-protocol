#include <algorithm>
#include <numeric>
#include <vector>

#include "Common/Core/Logger.hpp"
#include "Server/Core/OrderBook.hpp"

template<IsBook T, class _T>
bool OrderBook::add(T &_book, Price _price, Order &_order)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    _T cmp{};
    Event event;

    if constexpr (std::is_same_v<T, BidBook>)
        event.side = OrderType::Bid;
    else
        event.side = OrderType::Ask;
    event.sold = true;
    for (auto &[_key, _val] : _book) {
        if (_key != _price && cmp(_key, _price))
            break;

        event.price = _key;
        for (size_t i = 0; i < _val.size(); i++) {
            Order &order = _val.at(i);

            if (order.userId == _order.userId)
                continue;
            event.orderId = order.orderId;
            event.status = OrderStatus::Filled;
            event.userId = order.userId;
            event.quantity = 0;
            event.orgQty = order.quantity;
            if (order.quantity == _order.quantity) {
                Logger::Log("[OrderBook] (", m_name, ") {Add} Filled the order: ", order, ", price: ", _key);
                Logger::Log("[OrderBook] (", m_name, ") {Add-Incoming} Filled the order: ", _order);
                _order.quantity = 0;
                _val.erase(_val.begin() + i);
                _order.quantity = 0;
                m_output.append(event);
                return false;
            } else if (order.quantity < _order.quantity) {
                Logger::Log("[OrderBook] (", m_name, ") {Add} Filled the order: ", order, ", price: ", _key);
                Logger::Log("[OrderBook] (", m_name, ") {Add-Incoming} Partially filled the order: ", _order, ", new quantity: ", _order.quantity - order.quantity);
                _order.quantity -= order.quantity;
                _val.erase(_val.begin() + i);
                m_output.append(event);
            } else {
                Logger::Log("[OrderBook] (", m_name, ") {Add} Partially sold the order: ", order, ", new quantity: ", order.quantity - _order.quantity, ", price: ", _key);
                Logger::Log("[OrderBook] (", m_name, ") {Add-Incoming} Filled the order: ", _order);
                order.quantity -= _order.quantity;
                _order.quantity = 0;
                event.quantity = order.quantity;
                event.status = OrderStatus::PartiallyFilled;
                m_output.append(event);
                return false;
            }
        }
    }
    Logger::Log("[OrderBook] (Add) Finished order processing: ", _order, ", price: ", _price);
    return true;
}

template<IsBook T>
bool OrderBook::cancel(OrderIdMap<T> &_mapId, OrderId _orderId, bool _event)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    typename OrderIdMap<T>::iterator it = _mapId.find(_orderId);

    if (it != _mapId.end()) {
        if (_event) {
            Event event;

            if constexpr (std::is_same_v<T, BidBook>)
                event.side = OrderType::Bid;
            else
                event.side = OrderType::Ask;
            event.price = 0;
            event.orderId = _orderId;
            event.status = OrderStatus::Canceld;
            event.userId = it->second.second->userId;
            event.quantity = it->second.second->quantity;
            event.orgQty = it->second.second->quantity;
            event.sold = false;
            Logger::Log("[OrderBook] (", m_name, ") {Cancel} Sended event: "); // todo log
            m_output.append(event);
        }
        Logger::Log("[OrderBook] (", m_name, ") {Cancel} Sucefully canceled the order: ", _orderId);
        it->second.first->second.erase(it->second.second);
        _mapId.erase(it);
        return true;
    }
    return false;
}

template<IsBookCache T>
fix::MarketDataSnapshotFullRefresh OrderBook::refresh(T &_cache, size_t _depth)
{
    std::lock_guard<std::mutex> guard(m_mutex);


    size_t size = 0;
    fix::MarketDataSnapshotFullRefresh result;
    const std::string type = (std::is_same_v<T, cache_BidBook>) ? "0" : "1";

    switch (_depth) {
        case 0: size = _cache.size();
            break;
        case 1: size = std::max((size_t)1, (_cache.size() / 10) * 2);
            break;
        default: size = std::min(_depth, _cache.size());
    }
    Logger::Log("[OrderBook] size: ", size, ", cache: ", _cache.size());
    std::string ssize = std::to_string(size);
    std::string price{};
    std::string quantity{};
    std::string types{};

    for (const auto &[_price, _qty] : _cache) {
        if (size == 0)
            break;

        price += std::to_string(_price) + ",";
        types += type + ",";
        quantity += std::to_string(_qty) + ",";
        size--;
    }
    if (!price.empty())
        price.pop_back();
    if (!types.empty())
        types.pop_back();
    if (!quantity.empty())
        quantity.pop_back();
    Logger::Log("[OrderBook] Final version: { quantity: ", quantity, ", price: ", price, ", type: ", types, " }, size: ", ssize);
    result.set110_minQty(quantity);
    result.set267_noMDEntryTypes(ssize);
    result.set269_mDEntryType(types);
    result.set270_mDEntryPx(price);
    result.set55_symbol(m_name);
    return result;
}

template<IsBookCache T>
fix::MarketDataIncrementalRefresh OrderBook::update(T &_cache_orig, T &_cache, size_t _depth)
{
    size_t size = 0;
    fix::MarketDataIncrementalRefresh result;
    const std::string type = (std::is_same_v<T, cache_BidBook>) ? "0" : "1";

    switch (_depth) {
        case 0: size = _cache.size();
            break;
        case 1: size = std::max((size_t)1, (_cache.size() / 10) * 2);
            break;
        default: size = std::min(_depth, _cache.size());
    }
    std::string ssize = std::to_string(size);
    std::string prices{};
    std::string quantity{};
    std::string types{};
    std::string actions{};
    std::string symbols{};

    // need delete support
    for (const auto &[_price, _qty] : _cache) {
        if (size == 0)
            break;

        if (_cache_orig.contains(_price)) {
            if (_cache_orig.at(_price) == _qty)
                continue;
            actions += "1,";
            quantity += std::to_string(_cache_orig.at(_price) - _qty) + ",";
        } else {
            actions += "0,";
            quantity += std::to_string(_qty) + ",";
        }
        symbols += m_name + ",";
        prices += std::to_string(_price) + ",";
        types += type + ",";
        size--;
    }
    if (!types.empty())
        types.pop_back();
    if (!prices.empty())
        prices.pop_back();
    if (!actions.empty())
        actions.pop_back();
    if (!quantity.empty())
        quantity.pop_back();
    if (!symbols.empty())
        symbols.pop_back();
    result.set110_minQty(quantity);
    result.set267_noMDEntryTypes(ssize);
    result.set269_mDEntryType(types);
    result.set270_mDEntryPx(prices);
    result.set279_mDUpdateAction(actions);
    result.set55_symbol(symbols);
    return result;
}

template<IsBook T, IsBookCache _T>
void OrderBook::cache_on(T &_book, _T &_cache)
{
    _cache.clear();
    for (const auto &[_price, _orders] : _book) {
        Quantity total_qty = std::accumulate(_orders.begin(), _orders.end(), 0,
            [] (Quantity _sum, const Order& _order) {
                return _sum + _order.quantity;
            });
        _cache.emplace(_price, total_qty);
    }
}