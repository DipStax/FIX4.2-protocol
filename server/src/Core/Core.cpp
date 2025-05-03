#include "Common/Core/Logger.hpp"
#include "Server/Core/Core.hpp"

Core::Core(uint32_t _tcp_port, uint32_t _udp_port)
    : m_tcp_output("Standard TCP Output"),
    m_udp_output("UDP broadcast", _udp_port),
    m_logon("Logon handler", m_tcp_output.getInput()),
    m_router("Router", m_tcp_output.getInput(), m_logon.getInput()),
    m_tcp_input("Input Network", m_router.getInput(), m_tcp_output.getInput(), _tcp_port)
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
            m_udp_output.status();
            m_tcp_output.status();
            m_logon.status();
            for (auto &[_, _pip] : m_markets)
                _pip.status();
            m_router.status();
            m_tcp_input.status();
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
        m_tcp_input.stop();
        Logger::Log(m_tcp_input.getName(), " PU stoped");
        m_router.stop();
        Logger::Log(m_router.getName(), " PU stoped");
        m_logon.stop();
        Logger::Log(m_logon.getName(), " PU stoped");
        for (auto &[_name, _pip] : m_markets) {
            _pip.stop();
           Logger::Log(_pip.getName(), " PU stoped");
        }
        m_tcp_output.stop();
        Logger::Log(m_tcp_output.getName(), " PU stoped");
        m_udp_output.stop();
        Logger::Log(m_udp_output.getName(), " PU stoped");
        Logger::Log("All process unit are stoped");
    }
}

bool Core::internal_start()
{
    Logger::Log("Starting pipeline...");
    m_udp_output.start();
    Logger::Log(m_udp_output.getName(), " PU started");
    m_tcp_output.start();
    Logger::Log(m_tcp_output.getName(), " PU started");
    m_logon.start();
    Logger::Log(m_logon.getName(), " PU started");

    for (auto &[_name, _pip] : m_markets) {
        _pip.start();
        Logger::Log(_pip.getName(), " (", _name, ") PU started");
    }
    m_router.start();
    Logger::Log(m_router.getName(), " PU started");
    m_tcp_input.start();
    Logger::Log(m_tcp_input.getName(), " PU started");
    return false;
}

void Core::market_init()
{
    std::vector<std::string> name{ MARKET_NAME };


    for (std::string &_name : name) {
        m_markets.emplace(std::piecewise_construct,
            std::forward_as_tuple(_name),
            std::forward_as_tuple("Market " + _name, _name, m_udp_output.getInput(), m_tcp_output.getInput()));

        m_router.registerMarket(_name, m_markets.at(_name).getInput());
    }
}