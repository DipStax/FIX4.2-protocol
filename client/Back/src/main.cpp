#include "Client/Back/Core.hpp"

#include "Common/Log/Manager.hpp"
#include "Common/Log/Imp/Console.hpp"
#include "Common/Log/Imp/File.hpp"
#include "Common/Log/Imp/Buffer.hpp"

int main()
{
    log::Manager::registerNewLogger<log::imp::Console>("console");
    log::Manager::registerNewLogger<log::imp::File>("file");
    log::Manager::registerDefaultLogger<log::imp::Buffer>();

    Core core{8080, 8081};

    core.start();
    return 0;
}