#pragma once

#include <chrono>

template<class T>
struct Context : T
{
    Context() = default;
    Context(const Context<T> &&_data) noexcept;
    Context(const Context<T> &_data);
    template<class ...Ts>
    Context(std::chrono::system_clock::time_point _time, Ts &&..._args);
    ~Context() = default;

    Context<T> &operator=(Context<T> &&_data) noexcept;

    std::chrono::system_clock::time_point ReceiveTime;          ///< Receive time of the message
};

#include "Client/Back/ProcessUnit/data/Context.inl"