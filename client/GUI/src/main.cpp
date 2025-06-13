#include <QApplication>
#include <QThread>

#include "Client/GUI/UI/LoginScreen.hpp"

#include "Common/Log/Manager.hpp"
#include "Common/Log/Imp/Console.hpp"
#include "Common/Log/Imp/File.hpp"
#include "Common/Log/Imp/Buffer.hpp"

int main(int argc, char *argv[])
{
    logger::Manager::registerNewLogger<logger::imp::Console>("console");
    logger::Manager::registerNewLogger<logger::imp::File>("file");
    logger::Manager::registerDefaultLogger<logger::imp::Console>();

    qRegisterMetaType<net::Buffer>();

    QApplication app(argc, argv);
    ui::LoginScreen login;

    login.show();

    return app.exec();
}