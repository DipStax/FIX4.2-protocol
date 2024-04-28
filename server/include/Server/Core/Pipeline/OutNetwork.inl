#include <future>

#include "Common/Core/Logger.hpp"
#include "Server/Core/Pipeline/OutNetwork.hpp"

namespace pip
{
    template<class Func>
    requires IsProcessor<Func, OutNetworkInput &, std::vector<ClientSocket> &>
    OutNetwork<Func>::OutNetwork(std::vector<ClientSocket> &_clients, InOutNetwork &_input)
        : m_input(_input), m_clients(_clients)
    {
    }

    template<class Func>
    requires IsProcessor<Func, OutNetworkInput &, std::vector<ClientSocket> &>
    OutNetwork<Func>::~OutNetwork()
    {
        (void)(this->template stop());
    }

    template<class Func>
    requires IsProcessor<Func, OutNetworkInput &, std::vector<ClientSocket> &>
    bool OutNetwork<Func>::start()
    {
        if (!this->m_running)
            this->template tstart(this);
        Logger::Log("[OutNetwork] Running: ", this->m_running);
        return this->m_running;
    }

    template<class Func>
    requires IsProcessor<Func, OutNetworkInput &, std::vector<ClientSocket> &>
    void OutNetwork<Func>::loop()
    {
        Logger::SetThreadName(THIS_THREAD_ID, "Network Output");
        OutNetworkInput input;

        while (this->m_running) {
            if (!this->m_input.empty()) {
                input = std::move(this->m_input.pop_front());

                m_tp.enqueue([this, _input = std::move(input)] () mutable {
                    Func::run(_input, m_clients);
                });
            }
        }
    }
}