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
            return m_tick == fix42::TickDir::PlusTick || m_tick == fix42::TickDir::ZeroPlusTick;
        case fix42::Side::SellPlus:
            return m_tick == fix42::TickDir::MinusTick || m_tick == fix42::TickDir::ZeroMinusTick;
        default:
            return true;
    }
}

bool OrderBook::has(const OrderId &_orderId)
{
    return has(m_ask_side.idlist, _orderId) || has(m_bid_side.idlist, _orderId);
}

bool OrderBook::has(const OrderId &_orderId, fix42::Side _side)
{
    if (_side == fix42::Side::Buy || _side == fix42::Side::BuyMinus)
        return has(m_bid_side.idlist, _orderId);
    else if (_side == fix42::Side::Sell || _side == fix42::Side::SellPlus)
        return has(m_ask_side.idlist, _orderId);
    else
        Logger->log<logger::Level::Error>("Not supported side: ", static_cast<char>(_side));
    return false;
}

std::optional<Order> OrderBook::getOrder(const OrderId &_orderId)
{
    std::optional<IdList::Info> info = getOrderIdInfo(_orderId, fix42::Side::Buy);

    if (info.has_value())
        return *(info.value().Order);
    info = getOrderIdInfo(_orderId, fix42::Side::Sell);
    if (info.has_value())
        return *(info.value().Order);
    return std::nullopt;
}

std::vector<std::pair<Price, Quantity>> OrderBook::getSnapshot(uint32_t _depth, fix42::Side _side)
{
    std::vector<std::pair<Price, Quantity>> snapshot{};

    if (_side == fix42::Side::Buy) {
        getSnapshot(snapshot, _depth, m_bid_side);
    } else if (_side == fix42::Side::Sell) {
        getSnapshot(snapshot, _depth, m_ask_side);
    } else {
        throw std::runtime_error("Not supported side for snapshot: " + static_cast<char>(_side));
    }
    return snapshot;
}

void OrderBook::add(const OrderInfo &_order)
{
    std::pair<Quantity, Price> pair{};

    if (_order.order.side == fix42::Side::BuyMinus || _order.order.side == fix42::Side::Buy) {
        add(m_ask_side, m_bid_side, _order);
    } else if (_order.order.side == fix42::Side::SellPlus || _order.order.side == fix42::Side::Sell) {
        add(m_bid_side, m_ask_side, _order);
    } else {
        Logger->log<logger::Level::Error>("Order side not supported: ", static_cast<int>(_order.order.side));
        throw std::runtime_error("Side not supported, for add");
    }
}

bool OrderBook::cancel(const OrderId &_orderId, fix42::Side _side)
{
    std::optional<IdList::Info> info = getOrderIdInfo(_orderId, _side);

    if (info.has_value())
        return false;
    switch (_side) {
        case fix42::Side::Sell:
        case fix42::Side::SellPlus:
            Logger->log<logger::Level::Verbose>("Canceling on Sell side");
            removeFromIdMap(m_ask_side.idlist, _orderId);
            cancelOrder(m_ask_side, info.value());
            return true;
        case fix42::Side::Buy:
        case fix42::Side::BuyMinus:
            Logger->log<logger::Level::Verbose>("Canceling on Buy side");
            removeFromIdMap(m_bid_side.idlist, _orderId);
            cancelOrder(m_bid_side, info.value());
            return true;
        default:
            Logger->log<logger::Level::Error>("Unable to cancel on side: ", static_cast<char>(_side));
            throw std::runtime_error("Side not supported, for cancel");
    }
}

void OrderBook::lockReadOrderId(fix42::Side _side)
{
    switch (_side) {
        case fix42::Side::Sell:
        case fix42::Side::SellPlus:
            Logger->log<logger::Level::Verbose>("Locking bid id map");
            m_bid_side.idlist.mutex.lock_shared();
            break;
        case fix42::Side::Buy:
        case fix42::Side::BuyMinus:
            Logger->log<logger::Level::Verbose>("Locking ask id map");
            m_ask_side.idlist.mutex.lock_shared();
            break;
        default:
            Logger->log<logger::Level::Error>("Unable to lock side: ", static_cast<char>(_side));
            throw std::runtime_error("Side not supported, for lockReadOrderId");
    }
}

void OrderBook::unlockReadOrderId(fix42::Side _side)
{
    switch (_side) {
        case fix42::Side::Sell:
        case fix42::Side::SellPlus:
            Logger->log<logger::Level::Verbose>("Unlocking bid id map");
            m_bid_side.idlist.mutex.unlock_shared();
            break;
        case fix42::Side::Buy:
        case fix42::Side::BuyMinus:
            Logger->log<logger::Level::Verbose>("Unlocking ask id map");
            m_ask_side.idlist.mutex.unlock_shared();
            break;
        default:
            Logger->log<logger::Level::Fatal>("Unable to unlock side: ", static_cast<char>(_side));
            throw std::runtime_error("Side not supported, for unlockReadOrderId");
    }
}

bool OrderBook::has(IdList &_idlist, const OrderId &_orderId)
{
    std::shared_lock lock(_idlist.mutex);

    if (_idlist.list.contains(_orderId))
        return true;
    return false;
}

std::optional<OrderBook::IdList::Info> OrderBook::getOrderIdInfo(const OrderId &_orderId, fix42::Side _side)
{
    if (_side == fix42::Side::Buy || _side == fix42::Side::BuyMinus) {
        return getOrderIdInfo(m_bid_side.idlist, _orderId);
    } else if (_side == fix42::Side::Sell || _side == fix42::Side::SellPlus) {
        return getOrderIdInfo(m_ask_side.idlist, _orderId);
    } else {
        Logger->log<logger::Level::Fatal>("Side not supported: ", static_cast<char>(_side));
        throw std::runtime_error("Side not supported, for getOrderIdInfo");
    }
}

std::optional<OrderBook::IdList::Info> OrderBook::getOrderIdInfo(IdList &_idlist, const OrderId &_orderId)
{
    std::shared_lock lock(_idlist.mutex);

    if (_idlist.list.contains(_orderId))
        return _idlist.list.at(_orderId);
    return std::nullopt;
}

bool OrderBook::removeFromIdMap(IdList &_idlist, const OrderId &_orderid)
{
    std::unique_lock lock(_idlist.mutex);
    IdList::OrderIdMap::iterator it = _idlist.list.find(_orderid);

    if (it == _idlist.list.end())
        return false;
    _idlist.list.erase(it);
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
        Logger->log<logger::Level::Debug>("Main New average price: ", _main_event.order.avgPrice, " = (", cum_qty, " * ", _main_event.order.avgPrice, " + ", fill_qty, " * ", _price, ") / ", cum_total);
    }
    {
        const Quantity cum_qty = _order.originalQty - _order.remainQty;
        const Quantity cum_total = cum_qty + fill_qty;

        event.lastPrice = _price;
        event.lastQty = fill_qty;
        event.order.avgPrice = (cum_qty * _order.avgPrice + fill_qty * _price) / cum_total;
        Logger->log<logger::Level::Debug>("Order event New average price: ", event.order.avgPrice, " = (", cum_qty, " * ", _order.avgPrice, " + ", fill_qty, " * ", _price, ") / ", cum_total);
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
        m_tick = fix42::TickDir::PlusTick;
    } else if (m_lastprice < m_lastprice_diff) {
        m_tick = fix42::TickDir::MinusTick;
    } else {
        if (m_tick == fix42::TickDir::PlusTick || m_tick == fix42::TickDir::ZeroPlusTick)
            m_tick = fix42::TickDir::ZeroPlusTick;
        else
            m_tick = fix42::TickDir::ZeroMinusTick;
    }
    Logger->log<logger::Level::Info>("New tick side: ", m_tick);
}
