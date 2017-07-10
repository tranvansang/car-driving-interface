#include "mainwindow.h"
#include "drivingmodel.h"
#include <QApplication>
#include <QThread>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QThread modelThread;
    DrivingModel model(modelThread, &a);
    MainWindow w(model);
    w.show();

    return a.exec();
}
