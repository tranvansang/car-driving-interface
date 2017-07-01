#ifndef SETUPWINDOW_H
#define SETUPWINDOW_H

#include <QWidget>
#include <QTcpSocket>
#include "drivingmodel.h"

namespace Ui {
class SetupWindow;
}
class SetupWindow : public QWidget
{
    Q_OBJECT
public:
    explicit SetupWindow(QWidget *parent = nullptr, DrivingModel *drivingModel = nullptr);
    ~SetupWindow();

signals:

public slots:
private slots:
    void on_okButton_clicked();

private:
    Ui::SetupWindow *ui;
    DrivingModel *drivingModel;
};

#endif // SETUPWINDOW_H
