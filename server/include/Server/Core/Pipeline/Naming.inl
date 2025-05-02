#include "Server/Core/Pipeline/Naming.hpp"

template<class T>
Context<T>::Context(const Context<T> &&_data) noexcept
    : T(std::move(_data)), Client(std::move(_data.Client)), ReceiveTime(std::move(_data.ReceiveTime))
{
}

template<class T>
Context<T>::Context(const Context<T> &_data)
    : T(_data), Client(_data.Client), ReceiveTime(_data.ReceiveTime)
{
}

template<class T>
template<class ...Ts>
Context<T>::Context(const ClientStore::Client &_client, std::chrono::system_clock::time_point _time, Ts &&..._args)
    : T(std::forward<Ts>(_args)...), Client(_client), ReceiveTime(_time)
{
}

template<class T>
Context<T> &Context<T>::operator=(Context<T> &&_data) noexcept
{
    if (this != &_data) {
        Client = std::move(_data.Client);
        ReceiveTime = std::move(_data.ReceiveTime);
        T::operator=(std::move(_data));
    }
    return *this;
}
