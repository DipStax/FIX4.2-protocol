#include <algorithm>
#include <numeric>
#include <vector>

#include "Server/OrderBook.hpp"



template<class Comparator, IsBook BookType>
Quantity OrderBook::fillOnBook(BookType &_book, OrderIdMapBundle &_idmap, const OrderInfo &_order)
{
    Comparator cmp{};
    Event main_event{
        _order.price,
        fix42::ExecutionStatus::NewOrder,
        0.f,
        0.f,
        _order.order
    };

    for (auto &[_price, _list] : _book) {
        if (!cmp(_price, _order.price)) {
            Logger->log<logger::Level::Verbose>("Price invalidate comparator: ", _price, " <> ", _order.price);
            break;
        }

        if (!allowTick(_order.order.side)) {
            Logger->log<logger::Level::Info>("Order blocked by tick restriction: ", _order.order);
            return _order.order.remainQty;
        }

        for (size_t idx = 0; idx < _list.size(); idx++) {
            auto iterator = std::next(_list.begin(), idx);
            Order &order = *iterator;
            Event event{
                _price,
                fix42::ExecutionStatus::PartiallyFilled,
                _price,
                0.f,
                order
            };
            const Quantity main_diff = main_event.order.originalQty - main_event.order.remainQty;
            const Quantity diff = event.order.originalQty - event.order.remainQty;

            if (order.userId == _order.order.userId) {
                Logger->log<logger::Level::Verbose>("Skipping order because same user: ", order);
                continue;
            }
            main_event.lastPrice = _price;
            if (order.remainQty <= main_event.order.remainQty) {
                main_event.order.avgPrice = (main_event.order.avgPrice * main_diff + _price * order.remainQty) / main_event.order.originalQty;
                main_event.order.remainQty -= order.remainQty;
                main_event.lastQty = order.remainQty;

                event.order.avgPrice = (order.avgPrice * diff + _price * order.remainQty) / order.originalQty;
                event.order.remainQty = 0;
                event.order.status = fix42::ExecutionStatus::Filled;
                event.lastPrice = _price;
                event.lastQty = order.remainQty;

                _list.erase(iterator);
                idx--;
                removeFromIdMap(_idmap, order.orderId);
            } else {
                Logger->log<logger::Level::Info>("Fully filled order: ", _order.order);
                Logger->log<logger::Level::Debug>("From other side, partially filled order: ", order);
                // main_event.avgPrice = (main_event.avgPrice * main_diff + _price * main_event.remainQty) / main_event.orgQty;

                // order.remainQty -= main_event.remainQty;
                // event.remainQty = order.remainQty;

                // main_event.remainQty = 0;
            }

            m_event_output.push(std::move(event));

            computeTick(_price);

            if (main_event.order.remainQty == 0) {
                main_event.execStatus = fix42::ExecutionStatus::Filled;
                Logger->log<logger::Level::Debug>("Set event as execution type as: Filled");
                m_event_output.push(std::move(main_event));
                return 0.f;
            }
        }
    }
    if (main_event.order.remainQty != _order.order.originalQty) {
        main_event.order.status = fix42::ExecutionStatus::PartiallyFilled;
        Logger->log<logger::Level::Debug>("Set event as execution type as: PartiallyFilled");
        m_event_output.push(std::move(main_event));
    }
    return main_event.order.remainQty;
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