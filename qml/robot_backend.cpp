#include "robot_backend.hpp"

RobotBackend::RobotBackend(QObject* parent)
    : QObject(parent)
{
}

QString RobotBackend::connectSerial(const QString& device, int baudrate)
{
    connected_ = servo_.Feetech_Begin(device.toStdString(), baudrate);
    return connected_ ? "UART OK" : "UART False";
}

QString RobotBackend::moveServo(int id, int position)
{
    if (!connected_) {
        return "串口未连接";
    }

    bool ok = servo_.move(id, position);
    return ok ? "MOVE OK" : "MOVE False";
}

QString RobotBackend::setTorque(int id, bool on)
{
    if (!connected_) {
        return "串口未连接";
    }

    bool ok = servo_.Feetech_torque(id, on);
    if (!ok) {
        return on ? "扭矩打开失败" : "扭矩关闭失败";
    }

    return on ? "扭矩已打开" : "扭矩已关闭";
}

QString RobotBackend::readPosition(int id)
{
    if (!connected_) {
        return "串口未连接";
    }

    bool ok = servo_.Feetech_ReadPos(id);
    return ok ? "读取位置 OK，请看终端输出" : "读取位置失败";
}
