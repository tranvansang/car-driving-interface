#include "setupwindow.h"
#include "ui_setupwindow.h"

SetupWindow::SetupWindow(QWidget *parent, DrivingModel *drivingModel) :
    QWidget(parent),
    ui(new Ui::SetupWindow),
    drivingModel(drivingModel)
{
    ui->setupUi(this);
}

SetupWindow::~SetupWindow(){
    delete ui;
}

void SetupWindow::on_okButton_clicked()
{
    QString host = ui->hostText->text();
    QString port = ui->portText->text();
    drivingModel->connectServer(host, qint16(port.toInt()));
}
