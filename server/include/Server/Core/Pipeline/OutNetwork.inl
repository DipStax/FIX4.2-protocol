#include <future>

#include "Common/Core/Logger.hpp"
#include "Server/Core/Pipeline/OutNetwork.hpp"

namespace pip
{
    template<class Func>
    requires IsProcessor<Func, Context<OutNetworkInput> &>
    OutNetwork<Func>::OutNetwork(InOutNetwork &_input)
        : m_input(_input)
    {
    }

    template<class Func>
    requires IsProcessor<Func, Context<OutNetworkInput> &>
    void OutNetwork<Func>::runtime(std::stop_token _st)
    {
        Context<OutNetworkInput> input;

        while (!_st.stop_requested()) {
            if (!this->m_input.empty()) {
                input = std::move(this->m_input.pop_front());

                Logger::Log("New package to transmite");
                try {
                    m_tp.enqueue([this, _input = std::move(input)] () mutable {
                        Func::run(_input);
                    });
                } catch (std::exception &_e) {
                    Logger::Log("exception: ", _e.what());
                }
            }
        }
    }
}