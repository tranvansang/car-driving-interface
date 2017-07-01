#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent, DrivingModel *drivingModel) :
    QMainWindow(parent),
    drivingModel(drivingModel),
    drivingWindow(nullptr),
  setupWindow(new SetupWindow(this, drivingModel))
{
    setFixedSize(500, 500);
    setCentralWidget(setupWindow);

    connect(drivingModel, &DrivingModel::socketConnected, [this](){
       qDebug() << "Connected";
       drivingWindow = new DrivingWindow(this, this->drivingModel);
       setCentralWidget(drivingWindow);
       delete setupWindow;
       setupWindow = nullptr;
    });
}

MainWindow::~MainWindow()
{
    drivingModel->close();
    if (setupWindow) delete setupWindow;
    if (drivingWindow) delete drivingWindow;
}
