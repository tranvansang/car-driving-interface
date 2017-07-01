#ifndef DRIVINGWINDOW_H
#define DRIVINGWINDOW_H

#include <QWidget>
#include "drivingmodel.h"

namespace Ui {
class DrivingWindow;
}
class DrivingWindow : public QWidget
{
    Q_OBJECT
public:
    explicit DrivingWindow(QWidget *parent = nullptr, DrivingModel *drivingModel = nullptr);
    ~DrivingWindow();

signals:

public slots:
private slots:
    void on_startButton_clicked();

private:
    Ui::DrivingWindow *ui;
    void mouseMoveEvent(QMouseEvent *event);
    DrivingModel *drivingModel;

};

#endif // DRIVINGWINDOW_H
