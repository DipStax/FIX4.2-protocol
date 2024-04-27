#pragma once

#include <type_traits>

#include "Common/Core/Order.hpp"

template<class T, class _T>
concept IsBookOf = std::is_same_v<typename T::key_type, Price> &&
    std::is_same_v<typename T::mapped_type, _T> &&
    std::is_same_v<typename T::value_type, std::pair<const typename T::key_type, typename T::mapped_type>> &&
    requires(T &_map, typename T::iterator _it, Price _price) {
        { _map.size() } -> std::same_as<typename T::size_type>;
        { _map.begin() } -> std::same_as<typename T::iterator>;
        { _map.end() } -> std::same_as<typename T::iterator>;
        { _map.erase(_price) } -> std::same_as<typename T::size_type>;
        { _map.erase(_it) } -> std::same_as<typename T::iterator>;
        { _map.contains(_price) } -> std::same_as<bool>;
        { _map.at(_price) } -> std::same_as<typename T::mapped_type &>;
    };

template<class T>
concept IsMessageBase = std::is_base_of_v<fix::Message, T>;

#define MSG_PROP(_tag, _func)                       \
    template<class T>                               \
    concept Has## _tag  = IsMessageBase<T> &&       \
        requires (T _m, std::string &_v) {          \
            { _m._func(_v) } -> std::same_as<void>; \
        };

MSG_PROP(Symbol, set55_symbol)
MSG_PROP(Side, set54_side)
MSG_PROP(Quantity, set38_orderQty)
MSG_PROP(Price, set44_price)
MSG_PROP(OrderId, set11_clOrdID)
MSG_PROP(OriginalId, set41_origClOrdID)