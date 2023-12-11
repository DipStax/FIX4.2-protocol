#include <iostream>

#include "Core/Logger.hpp"
#include "Core/Core.hpp"

Core::Core(uint32_t _tcp_port, uint32_t _udp_port)
    : m_ob(), m_innet(m_client, m_nt_to_sr, _tcp_port), m_market(m_ob, m_sr_to_mk, m_mk_to_nt)
{
}

Core::~Core()
{
    stop();
}

void Core::start()
{
    Logger::Log("[Core] Starting...");
    m_running = true;
    internal_start();
    while (m_running)
    {
        try {
            m_innet.status();
            m_market.status();
        } catch (std::future_error &_e) {
            Logger::Log("[Core] Pipeline have crash: ", _e.what(), "\n\t> with the code: ", _e.code());
            stop();
        }
    }
    stop();
}

void Core::stop()
{
    if (m_running) {
        m_running = false;
        Logger::Log("[Core] Stoping...");
        m_market.stop();
        m_innet.stop();
        Logger::Log("[Core] All pipeline are stoped");
    }
}

void Core::internal_start()
{
    Logger::Log("[Core] Starting pipeline...");
    m_market.start();
    m_innet.start();
    Logger::Log("[Core] All pipeline are running");
}