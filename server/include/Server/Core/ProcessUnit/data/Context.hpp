#pragma once

#include "Server/Core/ClientStore.hpp"

template<class T>
struct Context : T
{
    Context() = default;
    Context(const Context<T> &&_data) noexcept;
    Context(const Context<T> &_data);
    template<class ...Ts>
    Context(const ClientStore::Client &_client, std::chrono::system_clock::time_point _time, Ts &&..._args);
    ~Context() = default;

    ClientStore::Client Client = nullptr;                      ///< Sender client information.
    std::chrono::system_clock::time_point ReceiveTime;

    Context<T> &operator=(Context<T> &&_data) noexcept;
};

#include "Server/Core/ProcessUnit/data/Context.inl"