#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("Bravikov");
    QCoreApplication::setApplicationName("VS Solution Explorer");
    QCoreApplication::setApplicationVersion("0.0.1");

    MainWindow w;
    w.show();
    return a.exec();
}
