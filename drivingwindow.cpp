#include "drivingwindow.h"
#include "ui_drivingwindow.h"
#include <QMouseEvent>
#include <QtMath>

DrivingWindow::DrivingWindow(QWidget *parent, DrivingModel *drivingModel) :
    QWidget(parent),
    ui(new Ui::DrivingWindow),
    drivingModel(drivingModel)
{
    ui->setupUi(this);
    ui->stopLabel->move((width() - ui->stopLabel->width()) / 2, static_cast<int>(height() - height() * DrivingModel::UPDOWN_THRESHOLD) - ui->stopLabel->height() / 2);
    drivingModel->driverInit(QPoint((width() - ui->handleLabel->width()) / 2, static_cast<int>(height() - height() * DrivingModel::UPDOWN_THRESHOLD - ui->handleLabel->height() / 2)),
                             size(), ui->stopLabel->pos(), ui->stopLabel->size());
}

DrivingWindow::~DrivingWindow(){
    delete ui;
}

void DrivingWindow::on_startButton_clicked()
{
    drivingModel->start();
}

void DrivingWindow::mouseMoveEvent(QMouseEvent *event){
    ui->handleLabel->move(event->pos().x() - ui->handleLabel->width() / 2, event->pos().y() - ui->handleLabel->height() / 2);
    drivingModel->updatePos(event->pos());
}
