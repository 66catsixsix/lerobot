#pragma once

#include <QObject>
#include <QString>

#include "feetech.hpp"

class RobotBackend : public QObject
{
    Q_OBJECT

public:
    explicit RobotBackend(QObject* parent = nullptr);

    Q_INVOKABLE QString connectSerial(const QString& device, int baudrate);
    Q_INVOKABLE QString moveServo(int id, int position);
    Q_INVOKABLE QString setTorque(int id, bool on);
    Q_INVOKABLE QString readPosition(int id);

private:
    Feetech servo_;
    bool connected_ = false;
};
