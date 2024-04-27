#include <iostream>

#include "Common/Core/Logger.hpp"
#include "Server/Core/Core.hpp"

Core::Core(uint32_t _tcp_port, uint32_t _udp_port)
    : m_innet(m_clients, m_q_action, m_q_tcp, _tcp_port),
        m_action(m_q_markets, m_q_action, m_q_data, m_q_tcp),
        m_data(m_markets, m_q_data, m_q_repdata),
        m_outnet(m_clients, m_q_tcp),
        m_outdata(m_clients, m_q_repdata),
        m_udp(m_q_udp, _udp_port)
{
    market_init();
}

Core::~Core()
{
    stop();
}

bool Core::start()
{
    Logger::Log("[Core] Starting...");
    m_running = true;
    if (!internal_start())
        return false;
    while (m_running)
    {
        try {
            m_udp.status();
            m_innet.status();
            for (auto &[_, _pip] : m_markets)
                _pip.status();
            m_action.status();
            m_data.status();
            m_outdata.status();
            m_outnet.status();
        } catch (std::future_error &_e) {
            Logger::Log("[Core] Pipeline have crash: ", _e.what(), "\n\t> with the code: ", _e.code());
            stop();
        }
    }
    stop();
    return true;
}

void Core::stop()
{
    if (m_running) {
        m_running = false;
        Logger::Log("[Core] Stoping...");
        while (m_udp.stop() != std::future_status::deferred) {}
        Logger::Log("[Core] UDP broadcast network exited");
        while (m_innet.stop() != std::future_status::deferred) {}
        Logger::Log("[Core] Input TCP network exited");
        while (m_action.stop() != std::future_status::deferred) {}
        Logger::Log("[Core] Action pipeline exited");
        while (m_data.stop() != std::future_status::deferred) {}
        Logger::Log("[Core] DataRefresh pipeline exited");
        for (auto &[_name, _pip] : m_markets) {
            for (MarketContainer::ThreadStatus status;
                std::get<0>(status) != std::future_status::deferred
                && std::get<1>(status) != std::future_status::deferred
                && std::get<2>(status) != std::future_status::deferred;
                status = _pip.stop()) {}
            Logger::Log("[Core] Market container exited, name: ", _name);
        }
        while (m_outdata.stop() != std::future_status::deferred) {}
        Logger::Log("[Core] UDP Output Notification Data Network exited");
        while (m_outnet.stop() != std::future_status::deferred) {}
        Logger::Log("[Core] Output network exited");
        while (m_udp.stop() != std::future_status::deferred) {}
        Logger::Log("[Core] UDP broadcast network exited");
        Logger::Log("[Core] All pipeline are stoped");
    }
}

bool Core::internal_start()
{
    Logger::Log("[Core] Starting pipeline...");
    if (!m_udp.start()) {
        Logger::Log("[Core] Failed to start UDP Broadcast Network");
        stop();
    } else if (!m_outnet.start()) {
        Logger::Log("[Core] Failed to start Output Network");
        stop();
    } else if (!m_outdata.start()) {
        Logger::Log("[Core] Failed to start Output Notification Data Network");
        stop();
    } else {
        for (auto &[_name, _pip] : m_markets) {
            if (!_pip.start()) {
                Logger::Log("[Core] Failed to start MarketContainer pipeline: ", _name);
                stop();
                return false;
            }
        }
        if (!m_action.start()) {
            Logger::Log("[Core] Failed to start Action pipeline");
            stop();
        } if (!m_data.start()) {
            Logger::Log("[Core] Failed to start DataRefresh pipeline");
            stop();
        } else if (!m_innet.start()) {
            Logger::Log("[Core] Failed to start Input Network");
            stop();
        } else {
            Logger::Log("[Core] All pipeline are running");
            return true;
        }
    }
    return false;
}

void Core::market_init()
{
    std::vector<std::string> name{ MARKET_NAME };

    for (std::string &_name : name) {
        m_markets.emplace(std::piecewise_construct,
            std::forward_as_tuple(_name),
            std::forward_as_tuple(_name, m_q_udp, m_q_tcp, m_clients));
        m_q_markets.emplace(_name, m_markets.at(_name).getInput());
    }
}