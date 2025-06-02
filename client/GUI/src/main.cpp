#include <QApplication>
#include <QMainWindow>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QMainWindow window;

    window.setWindowTitle("My Qt App");
    window.show();
    return app.exec();
}