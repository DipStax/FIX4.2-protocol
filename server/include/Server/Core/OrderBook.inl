#include <algorithm>
#include <numeric>
#include <vector>

#include "Server/Core/OrderBook.hpp"



template<class Comparator, IsBook BookType>
Quantity OrderBook::fillOnBook(BookBundle<BookType> &_book, OrderIdMapBundle &_idmap, const obs::OrderInfo &_order)
{
    Comparator cmp{};
    std::shared_lock lock_book(_book.Mutex);
    obs::Event event{
        _order.side,
        _order.order.orderId,
        _order.order.userId,
        _order.price,
        0.f,
        _order.order.quantity,
        _order.order.quantity,
        OrderStatusValue::New,
        ExecTypeValue::New
    };


    for (auto &[_price, _list] : _book.Book) {
        if (!cmp(_price, _order.price)) {
            Logger->log<logger::Level::Verbose>("Price invalidate comparator: ", _price, " <> ", _order.price);
            break;
        }

        std::shared_lock lock_idlist(_list.Mutex);

        for (size_t idx = 0; idx < _list.List.size(); idx++) {
            auto iterator = std::next(_list.List.begin(), idx);
            Order &order = *iterator;

            if (order.userId == _order.order.userId) {
                Logger->log<logger::Level::Verbose>("Skipping order because same user: ", order);
                continue;
            }
            if (order.quantity <= event.remainQty) {
                const uint32_t diff = event.orgQty - event.remainQty;

                if (order.quantity == event.remainQty) {
                    event.avgPrice = (event.avgPrice * diff + _price * event.remainQty) / (diff + order.quantity);
                    event.remainQty = 0;
                    Logger->log<logger::Level::Info>("Fully filled order: ", _order.order);
                    Logger->log<logger::Level::Debug>("From other side, fully filled order: ", order);
                } else {
                    event.avgPrice = (event.avgPrice * diff + _price * order.quantity) / (diff + order.quantity);
                    event.remainQty -= order.quantity;
                    Logger->log<logger::Level::Info>("Partially filled order: ", _order.order, ", new quantity: ", event.remainQty);
                    Logger->log<logger::Level::Debug>("From other side, fully filled order: ", order);
                }
                _list.List.erase(iterator);
                idx--;
                removeFromIdMap(_idmap, order.orderId);
            } else {
                Logger->log<logger::Level::Info>("Fully filled order: ", _order.order);
                Logger->log<logger::Level::Debug>("From other side, partially filled order: ", order);
                order.quantity -= event.remainQty;
                event.remainQty = 0;
            }
            if (event.remainQty == 0) {
                event.execStatus = ExecTypeValue::Filled;
                Logger->log<logger::Level::Debug>("Set event as execution type as: Filled");
                m_event_output.push(std::move(event));
                return 0;
            }
        }
    }
    if (event.remainQty != _order.order.quantity) {
        event.execStatus = ExecTypeValue::PartiallyFilled;
        Logger->log<logger::Level::Debug>("Set event as execution type as: PartiallyFilled");
        m_event_output.push(std::move(event));
    }
    return event.remainQty;
}

template<IsBook BookType>
void OrderBook::addToBook(BookBundle<BookType> &_book, OrderIdMapBundle &_idmap, Price _price, const Order &_order)
{
    std::unique_lock lock_book(_book.Mutex);
    obs::OrderListBundle &orderlist = _book.Book[_price];
    std::unique_lock lock_oderlist(orderlist.Mutex);

    orderlist.List.push_back(_order);
    std::unique_lock lock_idmap(_idmap.Mutex);

    Logger->log<logger::Level::Info>("New order place at price: ", _price, ", with order: ", _order);
    _idmap.IdList[_order.orderId] = { orderlist.List.end(), _price };
}