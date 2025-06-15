#include "Server/Core/Core.hpp"

Core::Core(uint32_t _tcp_port, uint32_t _udp_port)
    : m_udp_output(_udp_port),
    m_logon(m_tcp_output.getInput()),
    m_logout(m_tcp_output.getInput()),
    m_heartbeat(m_tcp_output.getInput()),
    m_router(m_tcp_output.getInput(),
        m_logon.getInput(),
        m_logout.getInput(),
        m_heartbeat.getInput()
    ),
    m_tcp_input(m_router.getInput(), m_tcp_output.getInput(), _tcp_port),
    Logger(logger::Manager::newLogger("Core"))
{
    market_init();
}

Core::~Core()
{
    stop();
}

bool Core::start()
{
    Logger->log<logger::Level::Info>("Starting server...");
    m_running = true;
    if (internal_start()) {
        m_running = false;
        return false;
    }
    while (m_running)
    {
        try {
            m_udp_output.status();
            // m_tcp_output.status();
            m_logon.status();
            m_logout.status();
            m_heartbeat.status();
            for (auto &[_, _pip] : m_markets)
                _pip.status();
            m_router.status();
            m_tcp_input.status();
        } catch (std::future_error &_e) {
            Logger->log<logger::Level::Fatal>("Pipeline have crash: ", _e.what(), "\n\t> with the code: ", _e.code());
            return false;
        } catch (std::exception &_e) {
            Logger->log<logger::Level::Fatal>("Pipeline have crash: ", _e.what());
            return false;
        }
    }
    stop();
    return true;
}

void Core::stop()
{
    if (m_running) {
        m_running = false;
        Logger->log<logger::Level::Info>("Stoping...");
        m_tcp_input.stop();
        m_router.stop();
        m_logon.stop();
        m_logout.stop();
        m_heartbeat.stop();
        // for (auto &[_name, _pip] : m_markets)
        //     _pip.stop();
        m_tcp_output.stop();
        // m_udp_output.stop();
        Logger->log<logger::Level::Info>("All process unit are stoped");
    }
}

bool Core::internal_start()
{
    Logger->log<logger::Level::Info>("Starting pipeline...");
    // m_udp_output.start();
    m_tcp_output.start();

    m_logon.start();
    m_logout.start();
    m_heartbeat.start();

    // for (auto &[_name, _pip] : m_markets)
    //     _pip.start();

    m_router.start();
    m_tcp_input.start();
    return false;
}

void Core::market_init()
{
    // std::vector<std::string> name{ MARKET_NAME };


    // for (std::string &_name : name) {
    //     m_markets.emplace(std::piecewise_construct,
    //         std::forward_as_tuple(_name),
    //         std::forward_as_tuple(_name, m_udp_output.getInput(), m_tcp_output.getInput()));

    //     m_router.registerMarket(_name, m_markets.at(_name).getInput());
    // }
}