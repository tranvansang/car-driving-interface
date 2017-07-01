#include "mainwindow.h"
#include "drivingmodel.h"
#include <QApplication>
#include <QThread>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QThread modelThread;
    DrivingModel model(&a, &modelThread);
    MainWindow w(nullptr, &model);
    w.show();

    return a.exec();
}
