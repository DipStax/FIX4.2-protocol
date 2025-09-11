#include <iostream>

#include "Client/Initiator/Config.hpp"
#include "Client/Initiator/Signal.hpp"

#include "Client/Initiator/Core.hpp"

#include "Shared/Log/Manager.hpp"
#include "Shared/Log/Imp/Buffer.hpp"
#include "Shared/Log/Imp/Console.hpp"
#include "Shared/Log/Imp/File.hpp"

int help(int _return)
{
    std::cout << "help: todo" << std::endl;
    return _return;
}

int main(int _ac, const char **_av)
{
    if (_ac != 2)
        return help(1);

    if (!std::strcmp(_av[1], "-h"))
        return help(0);

    setup_signal_handler();

    logger::Manager::registerNewLogger<logger::imp::Console>("console");
    logger::Manager::registerNewLogger<logger::imp::File>("file");
    logger::Manager::registerNewLogger<logger::imp::Buffer>("buffer");
    logger::Manager::registerDefaultLogger<logger::imp::Buffer>();

    Configuration<config::Global>::Load(_av[1], Configuration<config::Global>::Get());

    Core core{};

    core.start();

    logger::imp::Buffer::Stop();
    return 0;
}