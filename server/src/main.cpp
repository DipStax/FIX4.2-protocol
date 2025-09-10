#include "Server/Core.hpp"

#include "Shared/Log/Manager.hpp"
#include "Shared/Log/Imp/Console.hpp"
#include "Shared/Log/Imp/File.hpp"
#include "Shared/Log/Imp/Buffer.hpp"

int main(int _ac, const char **_av)
{
    std::ignore = _ac;
    std::ignore = _av;

    logger::Manager::registerNewLogger<logger::imp::Console>("console");
    logger::Manager::registerNewLogger<logger::imp::File>("file");
    logger::Manager::registerDefaultLogger<logger::imp::Buffer>();

    Core core{8080, 8081};

    return core.start();
}