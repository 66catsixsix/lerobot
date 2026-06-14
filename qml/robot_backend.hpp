#pragma once
#include <QObject>
#include <QString>
#include "feetech.hpp"

class RobotBackend : public QObject
{
    Q_OBJECT

public:
    explicit RobotBackend(QObject* parent = nullptr);
    Q_INVOKABLE QString test();
    Q_INVOKABLE QString connectSerival(QString device,int baudrate);
    Q_INVOKABLE QString setTorque(int id,bool on);
    Q_INVOKABLE QString moveServo(int id,int position);
    Q_INVOKABLE QString ReadPos(int id);
    Q_INVOKABLE QString startLearn();
    Q_INVOKABLE QString stopLearn();
    
private:
    Feetech servo_;
    bool connected_ = false;
    bool torque_status = false;
    bool learning_ = false;

    

};

