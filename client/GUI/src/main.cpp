#include <QApplication>

#include "Client/GUI/LoginForm.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    ui::LoginForm form;

    form.show();
    return app.exec();
}