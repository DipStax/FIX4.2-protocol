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