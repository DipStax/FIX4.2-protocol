#include "Client/Back/ProcessUnit/data/Context.hpp"

template<class T>
Context<T>::Context(const Context<T> &&_data) noexcept
    : T(std::move(_data)), ReceiveTime(std::move(_data.ReceiveTime))
{
}

template<class T>
Context<T>::Context(const Context<T> &_data)
    : T(_data), ReceiveTime(_data.ReceiveTime)
{
}

template<class T>
template<class ...Ts>
Context<T>::Context(std::chrono::system_clock::time_point _time, Ts &&..._args)
    : T(std::forward<Ts>(_args)...), ReceiveTime(_time)
{
}

template<class T>
Context<T> &Context<T>::operator=(Context<T> &&_data) noexcept
{
    if (this != &_data) {
        ReceiveTime = std::move(_data.ReceiveTime);
        T::operator=(std::move(_data));
    }
    return *this;
}
