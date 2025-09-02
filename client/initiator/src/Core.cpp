#include "Client/Initiator/Core.hpp"

#include "Shared/Log/Manager.hpp"

Core::Core()
    : Logger(logger::Manager::newLogger("Core"))
{
}

Core::~Core()
{
    stop();
}

bool Core::start()
{
    Logger->log<logger::Level::Info>("Starting Initiator...");
    m_front_handler.start();

    m_running = true;
    while (m_running)
    {
        try {
            // m_front_handler.status();
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
        m_front_handler.stop();
        m_running = false;
    }
}