#include <iostream>

#include "Client/Handler/Handler.hpp"

namespace io
{
    template<class T, class _T>
    Handler<T, _T>::~Handler()
    {
        stop();
    }

    template<class T, class _T>
    bool Handler<T, _T>::start()
    {
        if (!this->m_running) {
            m_thread = std::thread([this] () {
                loop();
            });
            this->m_running = true;
        }
        return this->m_running;
    }

    template<class T, class _T>
    bool Handler<T, _T>::stop()
    {
        this->m_running = false;
        if (m_thread.joinable()) {
            m_thread.join();
            return true;
        }
        return false;
    }

    template<class T, class _T>
    Handler<T, _T>::operator bool() const
    {
        return m_running;
    }

    template<class T, class _T>
    template<class ...Ts>
    void Handler<T, _T>::send(const Ts &&..._args)
    {
        m_input.push(std::forward<Ts>(_args)...);
    }

    template<class T, class _T>
    void Handler<T, _T>::send(const T &&_val)
    {
        m_input.push(std::move(_val));
    }

    template<class T, class _T>
    template<class ...Ts>
    void Handler<T, _T>::recv(const Ts &&..._args)
    {
        m_output.push(std::forward<Ts>(_args)...);
    }

    template<class T, class _T>
    void Handler<T, _T>::recv(const _T &&_val)
    {
        m_output.push(std::move(_val));
    }

    template<class T, class _T>
    bool Handler<T, _T>::empty(Side _side) const
    {
        if (_side == Side::Recv)
            return m_output.empty();
        return m_input.empty();
    }

    template<class T, class _T>
    T Handler<T, _T>::pop_front_send()
    {
        return m_input.pop_front();
    }

    template<class T, class _T>
    _T Handler<T, _T>::pop_front_recv()
    {
        return m_output.pop_front();
    }
}