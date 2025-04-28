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
    void OutNetwork<Func>::runtime(std::stop_token _st)
    {
        OutNetworkInput input;

        while (!_st.stop_requested()) {
            if (!this->m_input.empty()) {
                input = std::move(this->m_input.pop_front());

                m_tp.enqueue([this, _input = std::move(input)] () mutable {
                    Func::run(_input, m_clients);
                });
            }
        }
    }
}