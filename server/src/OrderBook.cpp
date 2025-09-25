#include <numeric>

#include "Server/OrderBook.hpp"

#include "Shared/Log/Manager.hpp"

OrderBook::OrderBook(const std::string &_name, ts::Queue<Event> &_event)
    : m_name(_name), m_event_output(_event), Logger(logger::Manager::newLogger("Market/" + m_name + "/OrderBook"))
{
}

const std::string &OrderBook::getSymbol() const
{
    return m_name;
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

bool OrderBook::has(const OrderId &_orderId, fix42::Side _side)
{
    if (_side == fix42::Side::Buy || _side == fix42::Side::BuyMinus)
    {
        std::shared_lock lock(m_ask_id.Mutex);

        if (m_ask_id.IdList.contains(_orderId))
            return true;
    } else if (_side == fix42::Side::Sell || _side == fix42::Side::SellPlus) {
        std::shared_lock lock(m_bid_id.Mutex);

        if (m_bid_id.IdList.contains(_orderId))
            return true;
    }
    return false;
}

Order OrderBook::getOrder(const OrderId &_orderId)
{
    {
        std::shared_lock lock(m_bid_id.Mutex);

        if (m_bid_id.IdList.contains(_orderId))
            return *(m_bid_id.IdList.at(_orderId).Order);
    }
    std::shared_lock lock(m_ask_id.Mutex);

    // todo can throw an exception if not found
    return *(m_ask_id.IdList.at(_orderId).Order);
}


bool OrderBook::add(const OrderInfo &_order)
{
    Quantity qty = 0;

    if (_order.order.side == fix42::Side::BuyMinus || _order.order.side == fix42::Side::Buy) {
        qty = fillOnBook<std::less_equal<Price>>(m_bid_book, m_bid_id, _order);
        if (qty != 0) {
            // todo change average price
            Order new_order{ _order.order.userId, _order.order.orderId, _order.order.originalQty, qty, 0.f, _order.order.side, fix42::OrderStatus::NewOrder };

            if (qty != _order.order.originalQty)
                new_order.status = fix42::OrderStatus::PartiallyFilled;
            addToBook(m_ask_book, m_ask_id, _order.price, new_order);
        }
    } else if (_order.order.side == fix42::Side::SellPlus || _order.order.side == fix42::Side::Sell) {
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

bool OrderBook::cancel(const OrderId &_orderId, fix42::Side _side)
{
    
}

void OrderBook::lockReadOrder(fix42::Side _side)
{
    switch (_side) {
        case fix42::Side::Sell:
        case fix42::Side::SellPlus:
            m_bid_id.Mutex.lock_shared();
            break;
        case fix42::Side::Buy:
        case fix42::Side::BuyMinus:
            m_ask_id.Mutex.lock_shared();
            break;
    }
}

void OrderBook::unlockReadOrder(fix42::Side _side)
{
    switch (_side) {
        case fix42::Side::Sell:
        case fix42::Side::SellPlus:
            m_bid_id.Mutex.unlock_shared();
            break;
        case fix42::Side::Buy:
        case fix42::Side::BuyMinus:
            m_ask_id.Mutex.unlock_shared();
            break;
    }
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

OrderBook::FillStatus OrderBook::fillOrder(Event &_main_event, Price _price, Order &_order)
{
    if (_order.userId == _main_event.order.userId)
        return FillStatus::Skipped;

    FillStatus fill_status = FillStatus::Skipped;
    Event event{
        fix42::ExecutionStatus::PartiallyFilled,
        _price,
        0.f,
        _order
    };

    const Quantity fill_qty = std::min(_order.remainQty, _main_event.order.remainQty);
    {
        const Quantity cum_qty = _main_event.order.originalQty - _main_event.order.remainQty;
        const Quantity cum_total = cum_qty + fill_qty;

        _main_event.lastPrice = _price;
        _main_event.lastQty = fill_qty;
        _main_event.order.avgPrice = (cum_qty * _main_event.order.avgPrice + fill_qty * _price) / cum_total;
        _main_event.order.remainQty -= fill_qty;
    }
    {
        const Quantity cum_qty = _order.originalQty - _order.remainQty;
        const Quantity cum_total = cum_qty + fill_qty;

        event.lastPrice = _price;
        event.lastQty = fill_qty;
        event.order.avgPrice = (cum_qty * _order.avgPrice + fill_qty * _price) / cum_total;
        event.order.remainQty -= fill_qty;
    }

    _order.remainQty -= fill_qty;
    _order.avgPrice = event.order.avgPrice;

    if (event.order.remainQty == 0) {
        _order.status = fix42::OrderStatus::Filled;
        event.execStatus = fix42::ExecutionStatus::Filled;
        fill_status = FillStatus::Filled;
    } else {
        _order.status = fix42::OrderStatus::PartiallyFilled;
        event.execStatus = fix42::ExecutionStatus::PartiallyFilled;
        fill_status = FillStatus::PartialyFilled;
    }

    m_event_output.push(std::move(event));
    return fill_status;
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
    Logger->log<logger::Level::Info>("New tick side: ", m_tick);
}
