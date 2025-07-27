#include <QApplication>
#include <QThread>

#include "Client/GUI/UI/screen/Login.hpp"
#include "Client/GUI/UI/screen/Main.hpp"

#include "Shared/Log/Manager.hpp"
#include "Shared/Log/Imp/Console.hpp"
#include "Shared/Log/Imp/File.hpp"
#include "Shared/Log/Imp/Buffer.hpp"

int main(int argc, char *argv[])
{
    logger::Manager::registerNewLogger<logger::imp::Console>("console");
    logger::Manager::registerNewLogger<logger::imp::File>("file");
    logger::Manager::registerDefaultLogger<logger::imp::Console>();

    qRegisterMetaType<net::Buffer>();

    QApplication app(argc, argv);
    ui::screen::Login login;
    ui::screen::Main main;

    if (login.exec() == QDialog::Accepted) {
        main.show();
    }
    return app.exec();
}