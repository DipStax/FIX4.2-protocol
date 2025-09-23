#include <numeric>

#include "Server/OrderBook.hpp"

#include "Shared/Log/Manager.hpp"

OrderBook::OrderBook(const std::string &_name, ts::Queue<Event> &_event)
    : m_name(_name), m_event_output(_event), Logger(logger::Manager::newLogger("Market/" + m_name + "/OrderBook"))
{
}

bool OrderBook::allowTick(fix42::Side _side)
{
    computeTick();
    switch (_side) {
        case fix42::Side::BuyMinus:
            return m_tick == TickDirection::PlusTick || m_tick == TickDirection::ZeroPlusTick;
        case fix42::Side::SellPlus:
            return m_tick == TickDirection::MinusTick || m_tick == TickDirection::ZeroMinusTick;
        default:
            return true;
    }
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

bool OrderBook::add(const OrderInfo &_order)
{
    Quantity qty = 0;

    if (_order.order.side == fix42::Side::BuyMinus) {
        qty = fillOnBook<std::less_equal<Price>>(m_bid_book, m_bid_id, _order);
        if (qty != 0) {
            // todo change average price
            Order new_order{ _order.order.userId, _order.order.orderId, _order.order.originalQty, qty, 0.f, _order.order.side, fix42::OrderStatus::NewOrder };

            if (qty != _order.order.originalQty)
                new_order.status = fix42::OrderStatus::PartiallyFilled;
            addToBook(m_ask_book, m_ask_id, _order.price, new_order);
        }
    } else if (_order.order.side == fix42::Side::SellPlus) {
        qty = fillOnBook<std::greater_equal<Price>>(m_ask_book, m_ask_id, _order);
        if (qty != 0) {
            Order new_order{ _order.order.userId, _order.order.orderId, _order.order.originalQty, qty, 0.f, _order.order.side, fix42::OrderStatus::NewOrder };

            if (qty != _order.order.originalQty)
                new_order.status = fix42::OrderStatus::PartiallyFilled;
            addToBook(m_bid_book, m_bid_id, _order.price, new_order);
        }
    } else {
        Logger->log<logger::Level::Error>("Order side not supported: ", static_cast<int>(_order.order.side));
        return false;
    }
    return true;
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

void OrderBook::computeTick(Price _new)
{
    m_lastprice_diff = m_lastprice;
    m_lastprice = _new;
    computeTick();
}

void OrderBook::computeTick()
{
    if (m_lastprice > m_lastprice_diff) {
        m_tick = TickDirection::PlusTick;
    } else if (m_lastprice < m_lastprice_diff) {
        m_tick = TickDirection::MinusTick;
    } else {
        if (m_tick == TickDirection::PlusTick || m_tick == TickDirection::ZeroPlusTick)
            m_tick = TickDirection::ZeroPlusTick;
        else
            m_tick = TickDirection::ZeroMinusTick;
    }
}
