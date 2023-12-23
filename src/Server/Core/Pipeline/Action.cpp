#include <future>

#include "Common/Core/Logger.hpp"
#include "Server/Core/Pipeline/Action.hpp"
#include "Server/Core/meta.hpp"

namespace pip
{
    Action::Action(NetToSerial &_input, SerialToMarket &_output, RawOutput &_raw)
        : m_input(_input), m_output(_output), m_raw(_raw)
    {
    }

    Action::~Action()
    {
        (void)stop();
    }

    bool Action::start()
    {
        if (!m_running)
            tstart(this);
        Logger::Log("[Action] Running: ", m_running);
        return m_running;
    }

    /// @brief Process all incoming raw message to make action
    void Action::loop()
    {
        Logger::SetThreadName(THIS_THREAD_ID, "Action convertion");

        SerialIn input;

        while (m_running) {
            if (!m_input.empty()) {
                input = m_input.pop_front();
                // processing of the message
            }
        }
    }
}