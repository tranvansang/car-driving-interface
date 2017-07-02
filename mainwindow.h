#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "drivingwindow.h"
#include "setupwindow.h"
#include "drivingmodel.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(DrivingModel &model, QWidget *parent = 0);
    ~MainWindow();

private:
    DrivingModel &drivingModel;
    DrivingWindow *drivingWindow;
    SetupWindow *setupWindow;
};

#endif // MAINWINDOW_H
