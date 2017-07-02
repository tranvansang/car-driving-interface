#include "mainwindow.h"

MainWindow::MainWindow(DrivingModel &drivingModel, QWidget *parent) :
    QMainWindow(parent),
    drivingModel(drivingModel),
    drivingWindow(nullptr),
    setupWindow(new SetupWindow(drivingModel, this))
{
    setFixedSize(500, 500);
    setCentralWidget(setupWindow);

    connect(&drivingModel, &DrivingModel::socketConnected, [this](){
       qDebug() << "Connected";
       drivingWindow = new DrivingWindow(this->drivingModel, this);
       setCentralWidget(drivingWindow);
       delete setupWindow;
       setupWindow = nullptr;
    });
}

MainWindow::~MainWindow()
{
    drivingModel.close();
    if (setupWindow) delete setupWindow;
    if (drivingWindow) delete drivingWindow;
}
