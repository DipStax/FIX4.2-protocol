#include <algorithm>
#include <numeric>
#include <vector>

#include "Server/OrderBook.hpp"



template<class Comparator, IsBook BookType>
Quantity OrderBook::fillOnBook(BookType &_book, OrderIdMapBundle &_idmap, const OrderInfo &_order)
{
    Comparator cmp{};
    Event main_event{
        _order.side,
        _order.order.orderId,
        _order.order.userId,
        _order.price,
        0.f,
        _order.order.quantity,
        _order.order.quantity,
        fix42::OrderStatus::NewOrder,
        fix42::ExecutionStatus::NewOrder
    };

    for (auto &[_price, _list] : _book) {
        if (!cmp(_price, _order.price)) {
            Logger->log<logger::Level::Verbose>("Price invalidate comparator: ", _price, " <> ", _order.price);
            break;
        }

        for (size_t idx = 0; idx < _list.size(); idx++) {
            auto iterator = std::next(_list.begin(), idx);
            Order &order = *iterator;

            if (order.userId == _order.order.userId) {
                Logger->log<logger::Level::Verbose>("Skipping order because same user: ", order);
                continue;
            }
            if (order.quantity <= main_event.remainQty) {
                const uint32_t diff = main_event.orgQty - main_event.remainQty;

                if (order.quantity == main_event.remainQty) {
                    main_event.avgPrice = (main_event.avgPrice * diff + _price * main_event.remainQty) / (diff + order.quantity);
                    main_event.remainQty = 0;
                    Logger->log<logger::Level::Info>("Fully filled order: ", _order.order);
                    Logger->log<logger::Level::Debug>("From other side, fully filled order: ", order);
                } else {
                    main_event.avgPrice = (main_event.avgPrice * diff + _price * order.quantity) / (diff + order.quantity);
                    main_event.remainQty -= order.quantity;
                    Logger->log<logger::Level::Info>("Partially filled order: ", _order.order, ", new quantity: ", main_event.remainQty);
                    Logger->log<logger::Level::Debug>("From other side, fully filled order: ", order);
                }
                _list.erase(iterator);
                idx--;
                removeFromIdMap(_idmap, order.orderId);
            } else {
                Logger->log<logger::Level::Info>("Fully filled order: ", _order.order);
                Logger->log<logger::Level::Debug>("From other side, partially filled order: ", order);
                order.quantity -= main_event.remainQty;
                main_event.remainQty = 0;
            }
            if (main_event.remainQty == 0) {
                main_event.execStatus = fix42::ExecutionStatus::Filled;
                Logger->log<logger::Level::Debug>("Set event as execution type as: Filled");
                m_event_output.push(std::move(main_event));
                return 0;
            }
        }
    }
    if (main_event.remainQty != _order.order.quantity) {
        main_event.execStatus = fix42::ExecutionStatus::PartiallyFilled;
        Logger->log<logger::Level::Debug>("Set event as execution type as: PartiallyFilled");
        m_event_output.push(std::move(main_event));
    }
    return main_event.remainQty;
}

template<IsBook BookType>
void OrderBook::addToBook(BookType &_book, OrderIdMapBundle &_idmap, Price _price, const Order &_order)
{
    OrderList &orderlist = _book[_price];

    orderlist.push_back(_order);

    std::unique_lock lock_idmap(_idmap.Mutex);

    Logger->log<logger::Level::Info>("New order place at price: ", _price, ", with order: ", _order);
    _idmap.IdList[_order.orderId] = { orderlist.end(), _price };
}