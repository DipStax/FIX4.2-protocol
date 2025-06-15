#pragma once

#include <type_traits>
#include <thread>

#define THIS_THREAD_ID std::this_thread::get_id()

namespace net
{
    class Socket;
}

template<class T>
concept IsKey = requires (T _val) {
    _val == _val;
};

template<class T>
concept IsEmptyCtor = std::is_default_constructible_v<T>;

template<class T>
concept IsNumeric = std::is_integral_v<T>;

template<class T>
concept IsSigned = IsNumeric<T> && std::is_signed_v<T>;

template<class T>
concept IsUnsigned = IsNumeric<T> && std::is_unsigned_v<T>;

template<class T>
concept IsFPN = std::is_floating_point_v<T>;