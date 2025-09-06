#include "Client/Back/Core.hpp"

#include "Shared/Log/Manager.hpp"
#include "Client/Back/FrontManager.hpp"
#include "Client/Shared/IPC/Helper.hpp"

#include "Client/Back/InitiatorManager.hpp"
#include "Client/Back/FrontManager.hpp"

Core::Core()
    : m_server(std::make_shared<net::INetTcp>()),
    m_tcp_output(m_server),
    m_builder(FrontManager::Instance().getMessageQueue(), m_tcp_output.getInput()),
    m_heartbeat(m_tcp_output.getInput()),
    m_auth(m_tcp_output.getInput()),
    m_execution(m_tcp_output.getInput()),
    m_router(
        m_tcp_output.getInput(),
        m_heartbeat.getInput(),
        m_auth.getInput(),
        m_execution.getInput()
    ),
    m_tcp_input(m_server, m_router.getInput()),
    Logger(logger::Manager::newLogger("Client/Core"))
{
    // if (!m_server->connect(net::Ip(127, 0, 0, 1), _tcp_port))
    //     Logger->log<logger::Level::Fatal>("Failed to connect to server");
    // Logger->log<logger::Level::Debug>("Notifying front of initialized status");
    FrontManager::Instance().send(ipc::Helper::Status(PUStatus::Initialize));
}

Core::~Core()
{
    stop();
}

bool Core::start()
{
    m_running = true;
    Logger->log<logger::Level::Info>("Starting client backend...");

    FrontManager::Instance().wait_frontend();
    m_tcp_output.start();
    m_builder.start();
    m_heartbeat.start();
    m_auth.start();
    m_execution.start();
    m_router.start();
    m_tcp_input.start();
    Logger->log<logger::Level::Debug>("Notifying front of running status");
    FrontManager::Instance().send(ipc::Helper::Status(PUStatus::Running));
    while (m_running)
    {
        try {
            m_tcp_output.status();
            m_builder.status();
            m_heartbeat.status();
            m_auth.status();
            m_execution.status();
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
    Logger->log<logger::Level::Debug>("Notifying front of stop status");
    FrontManager::Instance().send(ipc::Helper::Status(PUStatus::Stop));
    stop();
    return true;
}

void Core::stop()
{
    if (m_running) {
        m_running = false;
        Logger->log<logger::Level::Info>("Stoping...");
        m_tcp_input.stop();
        m_builder.stop();
        m_router.stop();
        m_execution.stop();
        m_auth.stop();
        m_heartbeat.stop();
        m_tcp_output.stop();
        Logger->log<logger::Level::Info>("All process unit are stoped");
    }
}