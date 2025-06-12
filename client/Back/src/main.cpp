#include "Client/Back/Core.hpp"

#include "Common/Log/Manager.hpp"
#include "Common/Log/Imp/Console.hpp"
#include "Common/Log/Imp/File.hpp"
#include "Common/Log/Imp/Buffer.hpp"

#include "Client/Back/FrontManager.hpp"

int main()
{
    logger::Manager::registerNewLogger<logger::imp::Console>("console");
    logger::Manager::registerNewLogger<logger::imp::File>("file");
    logger::Manager::registerDefaultLogger<logger::imp::Buffer>();

    FrontManager::Instance().wait_frontend();

    Core core{8080, 8081};

    core.start();
    return 0;
}