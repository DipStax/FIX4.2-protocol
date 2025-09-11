#include <QApplication>
#include <QThread>

#include "Client/GUI/UI/screen/Login.hpp"
#include "Client/GUI/UI/screen/Main.hpp"
#include "Client/GUI/Config.hpp"

#include "Shared/Log/Manager.hpp"
#include "Shared/Log/Imp/Console.hpp"
#include "Shared/Log/Imp/File.hpp"
#include "Shared/Log/Imp/Buffer.hpp"

int help(int _return)
{
    std::cout << "help: todo" << std::endl;
    return _return;
}

int main(int _ac, char *_av[])
{
    if (_ac != 2)
        return help(1);

    if (!std::strcmp(_av[1], "-h"))
        return help(0);

    logger::Manager::registerNewLogger<logger::imp::Console>("console");
    logger::Manager::registerNewLogger<logger::imp::File>("file");
    logger::Manager::registerNewLogger<logger::imp::Buffer>("buffer");
    logger::Manager::registerDefaultLogger<logger::imp::Buffer>();

    Configuration<config::Global>::Load(_av[1], Configuration<config::Global>::Get());

    qRegisterMetaType<net::Buffer>();

    QApplication app(_ac, _av);
    ui::screen::Login login;
    ui::screen::Main main;

    if (login.exec() == QDialog::Accepted) {
        main.show();
    }
    return app.exec();
}