#include "drivingmodel.h"

//socket update rate in seconds
#define UPDATE_RATE 0.3
#define MILISEC_PER_SEC 1000

// ui refresh rate
#define UI_REFRESH_RATE 0.01

//minimum speed scale to control wheel scale rate
#define WHEE_SCALE_MIN .5

// should turn traffic sign on when speed passes this threshold
#define SPEED_LIGHT_THRESHOLD 0
// same as SPEED_LIGHT_THRESHOLD
#define WHEEL_LIGHT_THRESHOLD 1

// when wheel lower than this value, eval wheel value based on WHEEL_LOW_RATE
#define WHEEL_THRESHOLD 20
#define WHEEL_LOW_RATE 0.5

// to prevent user from move mouse to edge of screen, when mouse offset pass this threshold
// set wheel value to be WHEEL_THRESHOLD_UP_VAL
#define WHEEL_THRESHOLD_UP 45
#define WHEEL_THRESHOLD_UP_VAL 20


DrivingModel::DrivingModel(QObject *parent, QThread *thread) :
    QObject(parent),
    pendingStart(false),
    currentParam(0),
    lock(),
    uiLock(),
    firstMove(true)
{
    if (thread) moveToThread(thread);
    connect(&socket, &QAbstractSocket::connected, [=]{
       qInfo() <<"connected";
       socketConnected();
    });
    connect(&socket, &QAbstractSocket::disconnected, [=](){
        qInfo() << "Disconnected";
    });
    connect(&socket, static_cast<void ( QTcpSocket::* )( QAbstractSocket::SocketError )>(&QAbstractSocket::error), [this](QAbstractSocket::SocketError){
       qWarning() <<" Error: " << socket.errorString();
    });
    for (int i = 0; i < NPARAM; i++)
        params[i][0] = params[i][1] = 0;
    params[WHEEL][0] = params[WHEEL][1] = 50;

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &DrivingModel::updateParam);
    timer->start(static_cast<int>(MILISEC_PER_SEC * UPDATE_RATE));
}

//accelerator vs decelerator area ratio
const double DrivingModel::UPDOWN_THRESHOLD = 0.4;

void DrivingModel::close(){
    timer->stop();
    delete timer;
    socket.close();
}

void DrivingModel::connectServer(QString const& host, quint16 port){
    socket.connectToHost(host, port, QIODevice::WriteOnly);
    qInfo() << "socket: connect to " << host <<  " port " << port;
    //socket->connectToHost("192.168.3.14", 5800, QIODevice::WriteOnly);
}

void DrivingModel::driverInit(QPoint const& newMouse, QSize const& winSize, QPoint const& stopPos, QSize const& stopSize){
    mouse = newMouse;
    this->winSize = winSize;
    this->stopPos = stopPos;
    this->stopSize = stopSize;
}

bool DrivingModel::shouldStop(QPoint const pos){
    return pos.x() > stopPos.x() && pos.x() < stopPos.x() + stopSize.width() &&
                pos.y() > stopPos.y() && pos.y() < stopPos.y() + stopSize.height();
}

QList<bool> DrivingModel::shouldLightOn(QPoint const pos){
    if (shouldStop(pos)){
        Q_ASSERT(Stop == 4);
        return QList<bool>({false, false, false, false, true});
    }
    bool should[5];
    //left -> up -> right -> down
    int threshold = static_cast<int>((1. - UPDOWN_THRESHOLD) * winSize.height());
    if (pos.y() > threshold){//decelerate
        should[Up] = false;
        should[Down] = evalSpeed(pos.y() - threshold, winSize.height() - threshold) > SPEED_LIGHT_THRESHOLD;
    }else { //accelerate
        should[Down] = false;
        should[Up] = evalSpeed(threshold - pos.y(), threshold) > SPEED_LIGHT_THRESHOLD;
    }
    int wheelOffset = evalWheel(abs(winSize.width() / 2 - pos.x()), winSize.width() / 2);
    if (pos.x() > winSize.width() / 2){//turn right
        should[Left] = false;
        should[Right] = wheelOffset > WHEEL_LIGHT_THRESHOLD;
    }else {
        should[Right] = false;
        should[Left] = wheelOffset > WHEEL_LIGHT_THRESHOLD;
    }
    should[Stop] = false;
    return QList<bool>::fromStdList(std::list<bool>(should, should + 5));
}

void DrivingModel::updatePos(QPoint const pos){
    if (mouse == pos)
        return;
    if (!uiLock.tryLock()){
        return;
    }
    if (firstMove){
        firstMove = false;
        pendingStart = true;
    }
    mouse = pos;
    if (shouldStop(mouse)){
        //stop
        params[ACCELERATOR][0] = 0;
        params[DECELERATOR][0] = 100;
        params[GEAR][0] = 0;
        params[WHEEL][0] = 50;
    }else{
        int threshold = static_cast<int>((1. - UPDOWN_THRESHOLD) * winSize.height());
        if (mouse.y() > threshold){//decelerate
            params[ACCELERATOR][0] = evalSpeed(mouse.y() - threshold, winSize.height() - threshold);
            params[GEAR][0] = 1;
        }else { //accelerate
            params[ACCELERATOR][0] = evalSpeed(threshold - mouse.y(), threshold);
            params[GEAR][0] = 0;
        }
        params[DECELERATOR][0] = 0;
        int wheelOffset = evalWheel(abs(winSize.width() / 2 - mouse.x()), winSize.width() / 2);
        if (mouse.x() > winSize.width() / 2){//turn right
            params[WHEEL][0] = 50 + wheelOffset;
        }else {
            params[WHEEL][0] = 50 - wheelOffset;
        }
    }
    uiLock.unlock();
}


//return from 0->100
int DrivingModel::evalSpeed(int offset, int maxVal){
    return offset * 100 / maxVal;
}

//return from 0->50
int DrivingModel::evalWheel(int offset, int maxVal){
    int scaled = offset * 50 / maxVal;
    if (scaled >= WHEEL_THRESHOLD_UP)
        return WHEEL_THRESHOLD_UP_VAL;
    if (scaled < WHEEL_THRESHOLD)
        return static_cast<int>(scaled * WHEEL_LOW_RATE);
    return static_cast<int>(WHEEL_THRESHOLD * WHEEL_LOW_RATE + (scaled - WHEEL_THRESHOLD) * (50 - WHEEL_THRESHOLD * WHEEL_LOW_RATE) / (50 - WHEEL_THRESHOLD));
}

qreal DrivingModel::rotateAngle(QPoint const& pos){
    int wheelOffset = evalWheel(abs(winSize.width() / 2 - pos.x()), winSize.width() / 2);
    return (pos.x() > winSize.width() / 2 ? 1. : -1.) * 90. * wheelOffset / 50;
}

//calculate wheel scale based on speed
qreal DrivingModel::wheelScale(QPoint const& pos){
    int threshold = static_cast<int>((1. - UPDOWN_THRESHOLD) * winSize.height());
    qreal t;
    if (pos.y() > threshold){//decelerate
        t = (100. - evalSpeed(pos.y() - threshold, winSize.height() - threshold)) / 100.;
    }else { //accelerate
        t = (100. - evalSpeed(threshold - pos.y(), threshold)) / 100.;
    }
    return WHEE_SCALE_MIN + (1. - WHEE_SCALE_MIN) * t;
}

void DrivingModel::updateParam(){
    if (!lock.tryLock()){
        return;
    }
    if (pendingStart){
        pendingStart = false;
        socket.write("start\n");
        socket.flush();
        qInfo() << "start\n";
        params[GEAR][0] = params[ACCELERATOR][0] = params[DECELERATOR][0] = 0;
        params[WHEEL][0] = 50;
    }else {
        for (int j = 0; j < NPARAM; j++){
            int i = (currentParam + j) % NPARAM;
            if (params[i][0] != params[i][1]){
                params[i][1] = params[i][0];
                currentParam = (currentParam + 1) % NPARAM;
                QString str("Wrong thing happened");
                switch (i){
                case GEAR:
                    str = QString(params[i][0] ? "shiftgear -1\n" : "shiftgear 1\n");
                    break;
                case ACCELERATOR:
                    str = QString("accelerator %1\n").arg(QString::number(params[i][0]));
                    break;
                case DECELERATOR:
                    str = QString("decelerator %1\n").arg(QString::number(params[i][0]));
                    break;
                case WHEEL:
                    str = QString("wheel %1\n").arg(QString::number(params[i][0]));
                    break;
                }
                socket.write(str.toUtf8());
                socket.flush();
                qInfo() << str.toUtf8();
                break;
            }
        }
    }
    lock.unlock();
}
void DrivingModel::start(){
    pendingStart = true;
}
