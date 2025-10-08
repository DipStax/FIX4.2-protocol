#include <algorithm>
#include <numeric>
#include <vector>

#include "Server/OrderBook.hpp"

template<class FillSide, class OrderSide>
void OrderBook::add(FillSide &_fillside, OrderSide &_orderside, const OrderInfo &_order)
{
    std::pair<Quantity, Price> pair = fillOnBook(_fillside, _order);

    if (pair.first != 0) {
        Order new_order{ _order.order.userId, _order.order.orderId, _order.order.originalQty, pair.first, pair.second, _order.order.side, fix42::OrderStatus::NewOrder };

        if (pair.first != _order.order.originalQty)
            new_order.status = fix42::OrderStatus::PartiallyFilled;
        addToBook(_orderside, _order.price, new_order);
    }
}

template<class Cmp>
std::pair<Quantity, Price> OrderBook::fillOnBook(OrderBookSide<Cmp> &_book_side, const OrderInfo &_order)
{
    Cmp cmp{};
    Event main_event{
        .execStatus = fix42::ExecutionStatus::NewOrder,
        .lastPrice = 0.f,
        .lastQty = 0.f,
        .order = _order.order
    };
    std::shared_lock lock(_book_side.mutex);

    for (auto &[_price, _list] : _book_side.book) {
        if (!cmp(_price, _order.price)) {
            Logger->log<logger::Level::Debug>("Price invalidate comparator: ", _price, " <> ", _order.price);
            break;
        }

        if (!allowTick(_order.order.side)) {
            Logger->log<logger::Level::Info>("Order blocked by tick restriction: ", _order.order);
            return {main_event.order.remainQty, main_event.order.avgPrice};
        }
        _list.mutex.lock_shared();
        for (size_t idx = 0; idx < _list.list.size(); idx++) {
            auto iterator = std::next(_list.list.begin(), idx);
            Order &order = *iterator;

            switch (fillOrder(main_event, _price, order)) {
                case FillStatus::Filled:
                    _list.mutex.unlock_shared();
                    _list.mutex.lock();
                    _list.cumQty -= main_event.lastQty;
                    _list.list.erase(iterator);
                    _list.mutex.unlock();
                    _list.mutex.lock_shared();
                    idx--;
                    removeFromIdMap(_book_side.idlist, order.orderId);
                    break;
                case FillStatus::PartialyFilled:
                    _list.cumQty -= main_event.lastQty;
                    break;
                case FillStatus::Skipped:
                    continue;
            }

            computeTick(_price);

            if (main_event.order.remainQty == 0) {
                main_event.execStatus = fix42::ExecutionStatus::Filled;
                Logger->log<logger::Level::Debug>("Set event as execution type as: Filled");
                m_event_output.push(std::move(main_event));
                return {0.f, 0.f};
            }
        }
        _list.mutex.unlock_shared();
    }
    if (main_event.order.remainQty != _order.order.originalQty) {
        main_event.execStatus = fix42::ExecutionStatus::PartiallyFilled;
        Logger->log<logger::Level::Debug>("Set event as execution type as: PartiallyFilled");
        m_event_output.push(std::move(main_event));
    }
    return {main_event.order.remainQty, main_event.order.avgPrice};
}

template<class Cmp>
void OrderBook::addToBook(OrderBookSide<Cmp> &_book, Price _price, const Order &_order)
{
    std::unique_lock lock_book(_book.mutex);
    typename OrderBookSide<Cmp>::PriceOrderList &orderlist = _book.book[_price];

    {
        std::unique_lock lock(orderlist.mutex);

        orderlist.list.push_back(_order);
        orderlist.cumQty += _order.remainQty;
    }
    std::unique_lock (_book.idlist.mutex);

    Logger->log<logger::Level::Info>("New order place at price: ", _price, ", with order: ", _order);
    _book.idlist.list[_order.orderId] = { --(orderlist.list.end()), _price };
}

template<class Cmp>
void OrderBook::cancelOrder(OrderBookSide<Cmp> &_book, const IdList::Info &_info)
{
    std::shared_lock lock_book(_book.mutex);
    typename OrderBookSide<Cmp>::PriceOrderList &orderlist = _book.book.at(_info.price);
    std::unique_lock lock(orderlist.mutex);

    Logger->log<logger::Level::Info>("Removing order: ", *(_info.Order), "at price: ", _info.price);
    orderlist.list.erase(_info.Order);
}