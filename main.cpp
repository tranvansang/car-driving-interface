#include "mainwindow.h"
#include "drivingmodel.h"
#include <QApplication>
#include <QThread>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DrivingModel model;
    QThread modelThread;
    model.moveToThread(&modelThread);
    MainWindow w(nullptr, &model);
    w.show();

    return a.exec();
}
