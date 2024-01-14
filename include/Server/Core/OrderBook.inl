#include <algorithm>
#include <vector>

#include "Server/Core/OrderBook.hpp"

template<IsBook T, class _T>
bool OrderBook::add(T &_book, Price _price, Order &_order)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    _T cmp{};
    Event event;

    if constexpr (std::is_same_v<T, BidBook>)
        event.side = OrderType::Ask;
    else
        event.side = OrderType::Bid;
    event.sold = true;
    for (auto &[_key, _val] : _book) {
        if (cmp(_key, _price))
            break;
        OrderList &ol = _book.at(_key);

        event.price = _key;
        for (size_t i = 0; i < ol.size(); i++) {
            Order &order = ol.at(i);

            event.orderId = order.orderId;
            event.status = OrderStatus::Filled;
            event.userId = order.userId;
            event.quantity = 0;
            event.orgQty = order.quantity;
            if (order.quantity == _order.quantity) {
                ol.erase(ol.begin() + i);
                m_output.append(event);
                return false;
            } else if (order.quantity < _order.quantity) {
                _order.quantity -= order.quantity;
                ol.erase(ol.begin() + i);
                m_output.append(event);
            } else {
                order.quantity -= _order.quantity;
                event.quantity = order.quantity;
                event.status = OrderStatus::PartiallyFilled;
                m_output.append(event);
                return false;
            }
        }
    }
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
            event.orderId = _orderId;
            event.status = OrderStatus::Canceld;
            event.userId = it->second.second->userId;
            event.quantity = it->second.second->quantity;
            event.orgQty = it->second.second->quantity;
            event.sold = false;
            m_output.append(event);
        }
        it->second.first->second.erase(it->second.second);
        _mapId.erase(it);
        return true;
    }
    return false;
}

template<IsBook T>
std::vector<Price> OrderBook::inter_getPrice(const T &_book)
{
    std::vector<Price> price(_book.size());
    size_t i = 0;
    std::lock_guard<std::mutex> guard(m_mutex);

    for (auto [_price, _order] : _book)
        price[i++] = _price;
    return price;
}