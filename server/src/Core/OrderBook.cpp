#include <numeric>

#include "Server/Core/OrderBook.hpp"
#include "Common/Message/ExecutionReport.hpp"
#include "Common/Log/Manager.hpp"

OrderBook::OrderBook(const std::string &_name)
    : m_name(_name)
{
}

std::pair<OrderStatus, Quantity> OrderBook::add(const obs::OrderInfo &_order)
{
    Quantity qty = 0;

    if (has(_order.order.orderId))
        return { OrderStatus::Rejected, 0 };
    if (_order.side == OrderType::Ask) {
        qty = fillOnBook<std::less_equal<Price>>(m_bid_book, m_bid_id, _order);
        if (qty != 0) {
            Order new_order{ _order.order.userId, _order.order.orderId, qty };
            addToBook(m_ask_book, m_ask_id, _order.price, new_order);
        }
    } else if (_order.side == OrderType::Bid) {
        qty = fillOnBook<std::greater_equal<Price>>(m_ask_book, m_ask_id, _order);
        if (qty != 0) {
            Order new_order{ _order.order.userId, _order.order.orderId, qty };
            addToBook(m_bid_book, m_bid_id, _order.price, new_order);
        }
    } else {
        Logger->log<logger::Level::Error>("Order side not supported: ", static_cast<int>(_order.side));
        return { OrderStatus::Rejected, 0 };
    }
    if (qty != 0) {
        if (qty == _order.order.quantity)
            return { OrderStatus::New, qty };
        return { OrderStatus::PartiallyFilled, qty };
    }
    return { OrderStatus::Filled, 0 };
}

bool OrderBook::has(const OrderId &_orderId)
{
    {
        std::shared_lock lock(m_ask_id.Mutex);

        if (m_ask_id.IdList.contains(_orderId))
            return true;
    }
    {
        std::shared_lock lock(m_bid_id.Mutex);

        if (m_bid_id.IdList.contains(_orderId))
            return true;
    }
    return false;
}

const std::string &OrderBook::getSymbol() const
{
    return m_name;
}

bool OrderBook::removeFromIdMap(OrderIdMapBundle &_idmap, const OrderId &_orderid)
{
    std::unique_lock lock(_idmap.Mutex);

    OrderIdMap::iterator it = _idmap.IdList.find(_orderid);
    if (it == _idmap.IdList.end())
        return false;
    _idmap.IdList.erase(it);
    return true;
}