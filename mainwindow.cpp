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
       drivingWindow = new DrivingWindow(this->drivingModel, this);
       setCentralWidget(drivingWindow);
       setupWindow->deleteLater();
       setupWindow = nullptr;
    });
}

MainWindow::~MainWindow()
{
    if (setupWindow) setupWindow->deleteLater();
    if (drivingWindow) delete drivingWindow;
}
