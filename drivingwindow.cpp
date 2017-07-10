#include "drivingwindow.h"
#include "ui_drivingwindow.h"
#include <QMouseEvent>
#include <QtMath>
#include <QLinearGradient>

#define DEACTIVE_OPACITY

DrivingWindow::DrivingWindow(DrivingModel &drivingModel, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DrivingWindow),
    drivingModel(drivingModel),
    handleImg(":/labels/resources/labels/wheel.png")
{
    ui->setupUi(this);
    ui->stopLabel->move((width() - ui->stopLabel->width()) / 2, static_cast<int>(height() - height() * DrivingModel::UPDOWN_THRESHOLD) - ui->stopLabel->height() / 2);
    drivingModel.driverInit(QPoint((width() - ui->handleLabel->width()) / 2, static_cast<int>(height() - height() * DrivingModel::UPDOWN_THRESHOLD - ui->handleLabel->height() / 2)),
                             size(), ui->stopLabel->pos(), ui->stopLabel->size());
    labels = QList<QLabel*>({ui->leftLabel, ui->upLabel, ui->rightLabel, ui->downLabel, ui->stopLabel});
    Q_ASSERT(DrivingModel::Left == 0);
    Q_ASSERT(DrivingModel::Up == 1);
    Q_ASSERT(DrivingModel::Right == 2);
    Q_ASSERT(DrivingModel::Down == 3);
    Q_ASSERT(DrivingModel::Stop == 4);
    for(int i = 0; i < 5; i++){
        labels[i]->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        labels[i]->setGraphicsEffect(opacityEffects + i);
    }
    ui->handleLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    handleSize = ui->handleLabel->size();
}

DrivingWindow::~DrivingWindow(){
    delete ui;
}

void DrivingWindow::on_startButton_clicked()
{
    drivingModel.start();
}

void DrivingWindow::mouseMoveEvent(QMouseEvent *event){
    ui->handleLabel->move(event->pos().x() - ui->handleLabel->width() / 2, event->pos().y() - ui->handleLabel->height() / 2);
    drivingModel.updatePos(event->pos());
    qreal angle = drivingModel.rotateAngle(event->pos());
    QPoint center = handleImg.rect().center();
    ui->handleLabel->setPixmap(QPixmap::fromImage(handleImg.transformed(QMatrix().translate(center.x(), center.y()).rotate(angle))));
    ui->handleLabel->resize(handleSize * drivingModel.wheelScale(event->pos()));

    QList<bool> shouldLightOn = drivingModel.shouldLightOn(event->pos());
    for (int i = 0; i < 5; i++){
        if (shouldLightOn[i])
            opacityEffects[i].setOpacity(1);
        else
            opacityEffects[i].setOpacity(.4);
    }
    if (shouldLightOn[DrivingModel::Stop]){
        ui->handleLabel->hide();
    }else{
        ui->handleLabel->show();
    }
}
