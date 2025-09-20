#include "Server/Core.hpp"
#include "Server/Config.hpp"
#include "Server/Signal.hpp"

#include "Shared/Log/Manager.hpp"
#include "Shared/Log/Imp/Console.hpp"
#include "Shared/Log/Imp/File.hpp"
#include "Shared/Log/Imp/Buffer.hpp"

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
    logger::Manager::registerDefaultLogger<logger::imp::Buffer>();

    logger::imp::File::Init("./logs/server");

    Configuration<config::Global>::Load(_av[1], Configuration<config::Global>::Get());

    Core core{};

    int result = core.start();

    logger::imp::Buffer::Stop();
    logger::imp::File::Deinit();
    return result;
}