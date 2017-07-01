#include "drivingmodel.h"

#define UPDATE_RATE 0.3
#define MILISEC_PER_SEC 1000
#define SPEED_THRESHOLD 20
#define SPEED_LOW_RATE 0.5
#define UI_REFRESH_RATE 0.01

DrivingModel::DrivingModel(QObject *parent) :
    QObject(parent),
    pendingStart(false),
    currentParam(0),
    lock(),
    timer(new QTimer(this)),
    uiLock(),
    firstMove(true)
{
    connect(&socket, &QAbstractSocket::connected, this, &DrivingModel::socketConnected);
    connect(&socket, &QAbstractSocket::disconnected, [](){
        qInfo() << "Disconnected";
    });
    connect(&socket, static_cast<void ( QTcpSocket::* )( QAbstractSocket::SocketError )>(&QAbstractSocket::error), [this](QAbstractSocket::SocketError){
       qWarning() <<" Error: " << socket.errorString();
    });
    for (int i = 0; i < NPARAM; i++)
        params[i][0] = params[i][1] = 0;
    params[WHEEL][0] = params[WHEEL][1] = 50;
    connect(timer, &QTimer::timeout, this, &DrivingModel::updateParam);
    timer->start(static_cast<int>(MILISEC_PER_SEC * UPDATE_RATE));
}

const double DrivingModel::UPDOWN_THRESHOLD = 0.4;

void DrivingModel::close(){
    socket.close();
}

void DrivingModel::connectServer(QString host, quint16 port){
    socket.connectToHost(host, port, QIODevice::WriteOnly);
    qInfo() << "socket: connect to " << host <<  " port " << port;
    //socket->connectToHost("192.168.3.14", 5800, QIODevice::WriteOnly);
}

void DrivingModel::driverInit(QPoint newMouse, QSize winSize, QPoint stopPos, QSize stopSize){
    mouse = newMouse;
    this->winSize = winSize;
    this->stopPos = stopPos;
    this->stopSize = stopSize;
}

void DrivingModel::updatePos(QPoint pos){
    if (mouse == pos)
        return;
    if (!uiLock.tryLock()){
        return;
    }
    if (firstMove){
        firstMove = false;
        pendingStart = true;
    }
    if (mouse.x() > stopPos.x() && mouse.x() < stopPos.x() + stopSize.width() &&
            mouse.y() > stopPos.y() && mouse.y() < stopPos.y() + stopSize.height()){
        //stop
        params[ACCELERATOR][0] = 0;
        params[DECELERATOR][0] = 100;
        params[GEAR][0] = 0;
        params[WHEEL][0] = 50;
    }else{
        int threshold = static_cast<int>((1 - UPDOWN_THRESHOLD) * winSize.height());
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
    if (scaled < SPEED_THRESHOLD)
        return static_cast<int>(scaled * SPEED_LOW_RATE);
    return static_cast<int>(SPEED_THRESHOLD * SPEED_LOW_RATE + (scaled - SPEED_THRESHOLD) * (50 - SPEED_THRESHOLD * SPEED_LOW_RATE) / (50 - SPEED_THRESHOLD));
}

void DrivingModel::updateParam(){
    if (!lock.tryLock()){
        return;
    }
    if (pendingStart){
        pendingStart = false;
        socket.write("start\n");
        socket.flush();
        qInfo() << "start";
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
                    str = QString(params[i][0] ? "shiftgear true" : "shiftgear false");
                    break;
                case ACCELERATOR:
                    str = QString("accelerator %1").arg(QString::number(params[i][0]));
                    break;
                case DECELERATOR:
                    str = QString("decelerator %1").arg(QString::number(params[i][0]));
                    break;
                case WHEEL:
                    str = QString("wheel %1").arg(QString::number(params[i][0]));
                    break;
                }
                socket.write(str.toUtf8());
                socket.flush();
                qInfo() << str;
                break;
            }
        }
    }
    lock.unlock();
}
void DrivingModel::start(){
    pendingStart = true;
}
