#include "Client/Core.hpp"

#include "Common/Log/Manager.hpp"

Core::Core(uint32_t _tcp_port, uint32_t _udp_port)
    : m_server(std::make_shared<net::tcp::Socket>()),
    m_tcp_input(m_server, m_tmp),
    Logger(log::Manager::newLogger("Core"))
{
    if (!m_server->connect(net::Ip(127, 0, 0, 1), _tcp_port))
        Logger->log<log::Level::Fatal>("Failed to connect to server");
}

Core::~Core()
{
    stop();
}

bool Core::start()
{
    m_running = true;
    Logger->log<log::Level::Info>("Starting client backend...");

    m_tcp_input.start();
    while (m_running)
    {
        try {
            m_tcp_input.status();
        } catch (std::future_error &_e) {
            Logger->log<log::Level::Fatal>("Pipeline have crash: ", _e.what(), "\n\t> with the code: ", _e.code());
            return false;
        } catch (std::exception &_e) {
            Logger->log<log::Level::Fatal>("Pipeline have crash: ", _e.what());
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
        Logger->log<log::Level::Info>("Stoping...");
        m_tcp_input.stop();
        Logger->log<log::Level::Info>("All process unit are stoped");
    }
}