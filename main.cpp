#include <QGst/Init>

#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QGst::init(&argc, &argv);

    MainWindow w;
    w.show();
    w.open();

    return a.exec();
}
