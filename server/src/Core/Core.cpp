#include <iostream>

#include "Common/Core/Logger.hpp"
#include "Server/Core/Core.hpp"

Core::Core(uint32_t _tcp_port, uint32_t _udp_port)
    : m_innet("Input Network", m_clients, m_q_action, m_q_tcp, _tcp_port),
    m_router("Router", m_q_action, m_q_data, m_q_tcp),
    m_data("Data Handler", m_markets, m_q_data, m_q_repdata),
    m_outnet("Standard TCP Output", m_clients, m_q_tcp),
    m_outdata("Subscribe TCP Output", m_clients, m_q_repdata),
    m_udp("UDP broadcast", m_q_udp, _udp_port)
{
    market_init();
}

Core::~Core()
{
    stop();
}

bool Core::start()
{
    Logger::Log("Starting...");
    m_running = true;
    if (internal_start())
        return false;
    while (m_running)
    {
        try {
            m_udp.status();
            m_innet.status();
            // for (auto &[_, _pip] : m_markets)
            //     _pip.status();
            m_router.status();
            m_data.status();
            m_outdata.status();
            m_outnet.status();
        } catch (std::future_error &_e) {
            Logger::Log("Pipeline have crash: ", _e.what(), "\n\t> with the code: ", _e.code());
            break;
        } catch (std::exception &_e) {
            Logger::Log("Pipeline have crash: ", _e.what());
            break;
        }
    }
    stop();
    return true;
}

void Core::stop()
{
    if (m_running) {
        m_running = false;
        Logger::Log("Stoping...");
        m_innet.stop();
        Logger::Log(m_innet.getName(), " PU exited");
        m_router.stop();
        Logger::Log(m_router.getName(), " PU exited");
        m_data.stop();
        Logger::Log(m_data.getName(), " PU exited");
        for (auto &[_name, _pip] : m_markets) {
            _pip.stop();
           Logger::Log(_pip.getName(), " PU exited");
        }
        m_outnet.stop();
        Logger::Log(m_outnet.getName(), " PU exited");
        m_udp.stop();
        Logger::Log(m_udp.getName(), " PU exited");
        Logger::Log("All process unit are stoped");
    }
}

bool Core::internal_start()
{
    Logger::Log("Starting pipeline...");
    m_udp.start();
    Logger::Log(m_udp.getName(), " PU started");
    m_outnet.start();
    Logger::Log(m_outnet.getName(), " PU started");
    m_outdata.start();
    Logger::Log(m_outdata.getName(), " PU started");

    for (auto &[_name, _pip] : m_markets) {
        _pip.start();
        Logger::Log(_pip.getName(), " (", _name, ") PU started");
    }
    m_data.start();
    Logger::Log(m_data.getName(), " PU started");
    m_router.start();
    Logger::Log(m_router.getName(), " PU started");
    m_innet.start();
    Logger::Log(m_innet.getName(), " PU started");
    // if (!m_udp.start()) {
    //     Logger::Log("[Core] Failed to start UDP Broadcast Network");
    //     stop();
    // } else if (!m_outnet.start()) {
    //     Logger::Log("[Core] Failed to start Output Network");
    //     stop();
    // } else if (!m_outdata.start()) {
    //     Logger::Log("[Core] Failed to start Output Notification Data Network");
    //     stop();
    // } else {
    //             Logger::Log("[Core] Failed to start MarketContain"Market " + 
        // if (!m_data.start()) {
        //     Logger::Log("[Core] Failed to start DataRefresh pipeline");
        //     stop();
        // } else if (!m_innet.start()) {
        //     Logger::Log("[Core] Failed to start Input Network");
        //     stop();
        // } else {
        //     Logger::Log("[Core] All pipeline are running");
        //     return true;
        // }
    // }
    return false;
}

void Core::market_init()
{
    std::vector<std::string> name{ MARKET_NAME };


    for (std::string &_name : name) {
        m_markets.emplace(std::piecewise_construct,
            std::forward_as_tuple(_name),
            std::forward_as_tuple("Market " + _name, _name, m_q_udp, m_q_tcp, m_clients));

        m_router.registerMarket(_name, m_markets.at(_name).getInput());
    }
}