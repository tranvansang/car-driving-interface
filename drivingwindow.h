#ifndef DRIVINGWINDOW_H
#define DRIVINGWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QGraphicsDropShadowEffect>
#include "drivingmodel.h"
#include <QGraphicsOpacityEffect>

namespace Ui {
class DrivingWindow;
}
class DrivingWindow : public QWidget
{
    Q_OBJECT
public:
    explicit DrivingWindow(DrivingModel &drivingModel, QWidget *parent = nullptr);
    ~DrivingWindow();

signals:

public slots:
private slots:
    void on_startButton_clicked();

private:
    Ui::DrivingWindow *ui;
    void mouseMoveEvent(QMouseEvent *event);
    DrivingModel &drivingModel;
    QImage handleImg;
    QSize handleSize;
    QGraphicsOpacityEffect opacityEffects[5];
    QList<QLabel*> labels;
};

#endif // DRIVINGWINDOW_H
