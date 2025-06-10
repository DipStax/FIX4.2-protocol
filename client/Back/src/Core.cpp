#include "Client/Back/Core.hpp"

#include "Common/Log/Manager.hpp"
#include "Client/Back/FrontManager.hpp"
#include "Client/Common/ipc/Helper.hpp"

Core::Core(uint32_t _tcp_port, uint32_t _udp_port)
    : m_server(std::make_shared<net::INetTcp>()),
    m_tcp_output(m_server),
    m_heartbeat(m_tcp_output.getInput()),
    m_auth(m_tcp_output.getInput()),
    m_router(
        m_tcp_output.getInput(),
        m_heartbeat.getInput(),
        m_auth.getInput()
    ),
    m_tcp_input(m_server, m_router.getInput()),
    Logger(log::Manager::newLogger("Core"))
{
    if (!m_server->connect(net::Ip(127, 0, 0, 1), _tcp_port))
        Logger->log<log::Level::Fatal>("Failed to connect to server");
    FrontManager::Instance().notify(ipc::Helper::Status(PUStatus::Initialize));
}

Core::~Core()
{
    stop();
}

bool Core::start()
{
    m_running = true;
    Logger->log<log::Level::Info>("Starting client backend...");


    m_tcp_output.start();
    m_heartbeat.start();
    m_auth.start();
    m_router.start();
    m_tcp_input.start();
    FrontManager::Instance().notify(ipc::Helper::Status(PUStatus::Running));
    while (m_running)
    {
        try {
            m_tcp_output.status();
            m_heartbeat.status();
            m_auth.status();
            m_router.status();
            m_tcp_input.status();
        } catch (std::future_error &_e) {
            Logger->log<log::Level::Fatal>("Pipeline have crash: ", _e.what(), "\n\t> with the code: ", _e.code());
            return false;
        } catch (std::exception &_e) {
            Logger->log<log::Level::Fatal>("Pipeline have crash: ", _e.what());
            return false;
        }
    }
    FrontManager::Instance().notify(ipc::Helper::Status(PUStatus::Stop));
    stop();
    return true;
}

void Core::stop()
{
    if (m_running) {
        m_running = false;
        Logger->log<log::Level::Info>("Stoping...");
        m_tcp_input.status();
        m_router.status();
        m_auth.status();
        m_heartbeat.status();
        m_tcp_output.status();
        Logger->log<log::Level::Info>("All process unit are stoped");
    }
}