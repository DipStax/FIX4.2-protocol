#include <algorithm>
#include <numeric>
#include <vector>

#include "Server/Core/OrderBook.hpp"

template<class Comparator, IsBook BookType>
Quantity OrderBook::fillOnBook(BookBundle<BookType> &_book, OrderIdMapBundle &_idmap, const obs::OrderInfo &_order)
{
    Comparator cmp{};
    Quantity qty = _order.order.quantity;
    std::shared_lock lock_book(_book.Mutex);

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
            if (order.quantity <= qty) {
                if (order.quantity == qty) {
                    qty = 0;
                    Logger->log<logger::Level::Info>("Fully filled order: ", _order.order);
                    Logger->log<logger::Level::Debug>("From other side, fully filled order: ", order);
                } else {
                    qty -= order.quantity;
                    Logger->log<logger::Level::Info>("Partially filled order: ", _order.order, ", new quantity: ", qty);
                    Logger->log<logger::Level::Debug>("From other side, fully filled order: ", order);
                }
                _list.List.erase(iterator);
                idx--;
                removeFromIdMap(_idmap, order.orderId);
            } else {
                Logger->log<logger::Level::Info>("Fully filled order: ", _order.order);
                Logger->log<logger::Level::Debug>("From other side, partially filled order: ", order);
                order.quantity -= qty;
                qty = 0;
            }
            if (qty == 0)
                return 0;
        }
    }
    return qty;
}

template<IsBook BookType>
void OrderBook::addToBook(BookBundle<BookType> &_book, OrderIdMapBundle &_idmap, Price _price, const Order &_order)
{
    std::unique_lock lock_book(_book.Mutex);
    obs::OrderListBundle &orderlist = _book.Book[_price];
    std::unique_lock lock_oderlist(orderlist.Mutex);

    orderlist.List.push_back(_order);
    std::unique_lock lock_idmap(_idmap.Mutex);

    _idmap.IdList[_order.orderId] = { orderlist.List.end(), _price };
}