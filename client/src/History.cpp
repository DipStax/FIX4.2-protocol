#include "Client/History.hpp"

void History::reset()
{
    m_orders.clear();
    m_history.clear();
}

void History::addOrder(const OrderClient &_order)
{
    Logger::Log("[History] Adding order: ", _order.orderId);
    m_orders.push_back(_order);
}

void History::addHistory(const OrderClient &_order)
{
    Logger::Log("[History] Adding order to history: ", _order.orderId);
    m_history.push_back(_order);
}

void History::removeOrder(const std::string &_orderId)
{
    Logger::Log("[History] Removing order: ", _orderId);
    auto it = std::find_if(m_orders.begin(), m_orders.end(), [&_orderId](const OrderClient &_order) {
        return _order.orderId == _orderId;
    });
    if (it != m_orders.end()) {
        m_orders.erase(it);
        Logger::Log("[History] Order removed: ", _orderId);
    }
    else {
        Logger::Log("[History] Order not found: ", _orderId);
    }
}

void History::replaceOrder(const std::string &_orderId, const OrderClient &_order)
{
    Logger::Log("[History] Replacing order: ", _orderId, " by ", _order.orderId);
    removeOrder(_orderId);
    addOrder(_order);
    Logger::Log("[History] Order replaced: ", _orderId, " by ", _order.orderId);
}

std::ostream &operator<<(std::ostream &_os, const std::vector<OrderClient> &_orders)
{
    for (auto &_order : _orders) {
        _os << "\t[" << ((_order.type == OrderType::Bid) ? "Buy" : "Sell") <<
        "] orderId: '" << _order.orderId << "', quantity: '" << _order.quantity <<
        "', price: '" << _order.price << "', status: '";
        switch (_order.status)
        {
            case OrderStatus::New:
                _os << "New";
                break;
            case OrderStatus::PartiallyFilled:
                _os << "PartiallyFilled";
                break;
            case OrderStatus::Filled:
                _os << "Filled";
                break;
            case OrderStatus::Canceld:
                _os << "Canceld";
                break;
            case OrderStatus::Replaced:
                _os << "Replaced";
                break;
            case OrderStatus::Pending:
                _os << "Pending";
                break;
            case OrderStatus::Rejected:
                _os << "Rejected";
                break;
        }
        _os << "', symbol: '" << _order.symbol << "'" << std::endl;
    }
    return _os;
}

std::ostream &operator<<(std::ostream &_os, const History &_history)
{
    _os << "History:\n";
    _os << "\tOrders:\n";
    _os << "\t" << _history.m_orders;
    _os << "\tHistory:\n";
    _os << "\t" << _history.m_history;
    return _os;
}