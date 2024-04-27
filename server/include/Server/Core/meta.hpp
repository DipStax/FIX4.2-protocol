#pragma once

#include <concepts>
#include <memory>
#include <type_traits>

#include "Common/Core/meta.hpp"
#include "Common/Core/Order.hpp"
#include "Common/Thread/Queue.hpp"

namespace meta
{
    namespace imp
    {
        template<template<typename ...> class T, class _T>
        struct is_base_of_template
        {
            template<class ...Ts>
            static constexpr std::true_type  test(const T<Ts...> *);
            static constexpr std::false_type test(...);
            using type = decltype(test(std::declval<_T*>()));
        };

        template<class ...Ts>
        struct same_as_tuple;

        template<class T, class ...Ts, class _T, class ..._Ts>
        struct same_as_tuple<std::tuple<T, Ts...>, _T, _Ts...>
        {
            using type = std::false_type;
        };

        template<class T, class ...Ts, class ..._Ts>
        struct same_as_tuple<std::tuple<T, Ts...>, T, _Ts...>
        {
            using type = typename same_as_tuple<std::tuple<Ts...>, _Ts...>::type;
        };

        template<class T, class _T>
        struct same_as_tuple<std::tuple<T>, _T>
        {
            using type = std::false_type;
        };
        template<class T>
        struct same_as_tuple<std::tuple<T>, T>
        {
            using type = std::true_type;
        };
    }

    template<class ...Ts>
    struct same_as_tuple
    {
        using type = typename imp::same_as_tuple<Ts...>::type;
    };

    template<template<typename ...> class base, class derived>
    using is_base_of_template = typename imp::is_base_of_template<base, derived>::type;
}


template<class ...Ts>
concept SameAsTuple = meta::same_as_tuple<Ts...>::type::value;

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
concept IsBook = IsBookOf<T, OrderList>;

template<class T>
concept IsBookCache = IsBookOf<T, Quantity>;

template<class T, class _T>
concept SocketClient = IsSocket<_T> && (std::is_same_v<std::shared_ptr<_T>, T> || requires (std::shared_ptr<_T> _s) {
    { T(_s) };
});

template<class T, class ...Ts>
concept IsProcessor = requires (Ts... _args) {
    { T::run(std::forward<Ts>(_args)...) } -> std::same_as<bool>;
};

template<class T>
concept IsTSQueue = meta::is_base_of_template<ts::Queue, T>::type::value;