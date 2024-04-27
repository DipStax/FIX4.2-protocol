#include <algorithm>

#include "Client/Processor/OrderBook.hpp"
#include "Common/Core/Utils.hpp"

namespace data
{
    template<class T>
    std::vector<T> extract(fix::Serializer::AnonMessage &_msg, const std::string &_tag)
    {
        std::vector<std::string> list = utils::split<','>(_msg.at(_tag));
        std::vector<T> result{};

        std::for_each(list.begin(), list.end(), [&result] (const std::string &_value) {
            if constexpr (std::is_same_v<T, std::string>)
                result.emplace_back(_value);
            else
                result.emplace_back(utils::to<T>(_value));
        });
        return result;
    }
}

namespace proc
{
    template<class T, class _T>
    void OrderBook::sync_book(T &_origine, _T &_target, SyncFn _sync)
    {
        for (auto [_sym, _book] : _origine)
            for (auto &[_price, _qty] : _book)
                functional_sync<T>(_target, _sym, _price, _qty, _sync);
    }

    // turn this function into class later
    template<class T>
    void OrderBook::functional_sync(T &_map, const std::string &_sym, Price _price, Quantity _qty, SyncFn _sync)
    {
        if (_qty != 0)
            _map[_sym][_price] = _sync(_qty, _map[_sym][_price]);
        else
            _map[_sym].erase(_price);
    }

    template<class T>
    requires IsBookOf<T, Quantity>
    void OrderBook::displayBook(const T &_book) const
    {
        std::cout << "Total number of price: " << _book.size() << std::endl;
        std::cout << "Price\t\tQuantity" << std::endl;
        for (const auto &[_price, _qty] : _book)
            std::cout << _price << "\t|\t" << _qty << std::endl;
    }
}