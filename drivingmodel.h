#ifndef DRIVINGMODEL_H
#define DRIVINGMODEL_H

#include <QObject>
#include <QTcpSocket>
#include <QMutex>
#include <QTimer>
#include <QPoint>
#include <QSize>
#include <QThread>

#define NPARAM 4
#define GEAR 0
#define ACCELERATOR 1
#define DECELERATOR 2
#define WHEEL 3

class DrivingModel : public QObject
{
    Q_OBJECT
    Q_ENUMS(Directions)
public:
    enum Directions {Left = 0, Up, Right, Down, Stop};
    explicit DrivingModel(QObject *parent = nullptr, QThread *thread = nullptr);
    void close();
    void connectServer(QString const&, quint16);
    void start();
    void updatePos(QPoint const);
    void driverInit(QPoint const&, QSize const&, QPoint const&, QSize const&);
    static const double UPDOWN_THRESHOLD;
    qreal rotateAngle(QPoint const&);
    qreal wheelScale(QPoint const&);
    QList<bool> shouldLightOn(QPoint const);


signals:
    void socketConnected();

public slots:
private:
    bool shouldStop(QPoint const);
    void updateParam();
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
