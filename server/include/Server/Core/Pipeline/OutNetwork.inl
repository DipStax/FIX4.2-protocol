#include <future>

#include "Common/Core/Logger.hpp"
#include "Server/Core/Pipeline/OutNetwork.hpp"

namespace pip
{
    template<IsSocket T, class _T, IsTSQueue __T, class ___T>
    requires SocketClient<___T, T> && IsProcessor<_T, typename __T::type &, std::vector<___T> &>
    OutNetwork<T, _T, __T, ___T>::OutNetwork(std::vector<___T> &_clients, __T &_input)
        : m_input(_input), m_clients(_clients)
    {
    }

    template<IsSocket T, class _T, IsTSQueue __T, class ___T>
    requires SocketClient<___T, T> && IsProcessor<_T, typename __T::type &, std::vector<___T> &>
    OutNetwork<T, _T, __T, ___T>::~OutNetwork()
    {
        (void)(this->template stop());
    }

    template<IsSocket T, class _T, IsTSQueue __T, class ___T>
    requires SocketClient<___T, T> && IsProcessor<_T, typename __T::type &, std::vector<___T> &>
    bool OutNetwork<T, _T, __T, ___T>::start()
    {
        if (!this->m_running)
            this->template tstart(this);
        Logger::Log("[OutNetwork] Running: ", this->m_running);
        return this->m_running;
    }

    template<IsSocket T, class _T, IsTSQueue __T, class ___T>
    requires SocketClient<___T, T> && IsProcessor<_T, typename __T::type &, std::vector<___T> &>
    void OutNetwork<T, _T, __T, ___T>::loop()
    {
        Logger::SetThreadName(THIS_THREAD_ID, "Network Output");
        typename __T::type input;

        while (this->m_running) {
            if (!this->m_input.empty()) {
                input = std::move(this->m_input.pop_front());

                m_tp.enqueue([this, _input = std::move(input)] () mutable {
                    _T::run(_input, m_clients);
                });
            }
        }
    }
}