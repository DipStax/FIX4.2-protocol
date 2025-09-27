#include <algorithm>
#include <numeric>
#include <vector>

#include "Server/OrderBook.hpp"



template<class Comparator, IsBook BookType>
Quantity OrderBook::fillOnBook(BookType &_book, OrderIdMapBundle &_idmap, const OrderInfo &_order)
{
    Comparator cmp{};
    Event main_event{
        fix42::ExecutionStatus::NewOrder,
        0.f,
        0.f,
        _order.order
    };

    for (auto &[_price, _list] : _book) {
        if (!cmp(_price, _order.price)) {
            Logger->log<logger::Level::Debug>("Price invalidate comparator: ", _price, " <> ", _order.price);
            break;
        }

        if (!allowTick(_order.order.side)) {
            Logger->log<logger::Level::Info>("Order blocked by tick restriction: ", _order.order);
            return _order.order.remainQty;
        }
        Price price = (_order.order.side == fix42::Side::Sell || _order.order.side == fix42::Side::SellPlus) ? _order.price : _price;

        for (size_t idx = 0; idx < _list.size(); idx++) {
            auto iterator = std::next(_list.begin(), idx);
            Order &order = *iterator;

            switch (fillOrder(main_event, price, order)) {
                case FillStatus::Filled:
                    _list.erase(iterator);
                    idx--;
                    removeFromIdMap(_idmap, order.orderId);
                case FillStatus::PartialyFilled:
                    break;
                case FillStatus::Skipped:
                    continue;
            }

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
    _idmap.IdList[_order.orderId] = { --(orderlist.end()), _price };
}