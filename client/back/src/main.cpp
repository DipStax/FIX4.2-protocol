#include "Client/Back/Core.hpp"
#include "Client/Back/FrontManager.hpp"
#include "Client/Back/InitiatorManager.hpp"

#include "Shared/Log/Manager.hpp"
#include "Shared/Log/Imp/Console.hpp"
#include "Shared/Log/Imp/File.hpp"
#include "Shared/Log/Imp/Buffer.hpp"

int main()
{
    logger::Manager::registerNewLogger<logger::imp::Console>("console");
    logger::Manager::registerNewLogger<logger::imp::File>("file");
    logger::Manager::registerNewLogger<logger::imp::Buffer>("buffer");
    logger::Manager::registerDefaultLogger<logger::imp::Buffer>();

    InitiatorManager::Instance().connect();
    FrontManager::Instance().wait_frontend();

    Core core{};

    core.start();
    return 0;
}