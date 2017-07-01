#ifndef DRIVINGMODEL_H
#define DRIVINGMODEL_H

#include <QObject>
#include <QTcpSocket>
#include <QMutex>
#include <QTimer>
#include <QPoint>
#include <QSize>

#define NPARAM 4
#define GEAR 0
#define ACCELERATOR 1
#define DECELERATOR 2
#define WHEEL 3

class DrivingModel : public QObject
{
    Q_OBJECT
public:
    explicit DrivingModel(QObject *parent = nullptr);
    void close();
    void connectServer(QString, quint16);
    void start();
    void updatePos(QPoint);
    void driverInit(QPoint, QSize, QPoint, QSize);
    static const double UPDOWN_THRESHOLD;


signals:
    void socketConnected();

public slots:
    void updateParam();
private:
    QTcpSocket socket;
    QPoint mouse;
    bool pendingStart;
    int params[NPARAM][2];
    int currentParam;
    QMutex lock;
    QTimer *timer;
    QMutex uiLock;
    static int evalSpeed(int, int);
    static int evalWheel(int, int);
    QSize winSize, stopSize;
    QPoint stopPos;
    bool firstMove;
};

#endif // DRIVINGMODEL_H
