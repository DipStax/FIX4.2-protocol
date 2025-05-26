#include "Server/Core/Core.hpp"

#include "Common/Log/Manager.hpp"
#include "Common/Log/Imp/Console.hpp"
#include "Common/Log/Imp/File.hpp"
#include "Common/Log/Imp/Buffer.hpp"

int main(int _ac, const char **_av)
{
    std::ignore = _ac;
    std::ignore = _av;

    log::Manager::registerNewLogger<log::imp::Console>("console");
    log::Manager::registerNewLogger<log::imp::File>("file");
    log::Manager::registerDefaultLogger<log::imp::Buffer>();

    Core core{8080, 8081};

    return core.start();
}