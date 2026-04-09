#include "mainwindow.h"

#include <QApplication>
#include <QDebug>

#include "ip_locator.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.showMaximized();
    return a.exec();
}
