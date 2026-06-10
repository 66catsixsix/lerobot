#include "robot_backend.hpp"

RobotBackend::RobotBackend(QObject* parent)
    : QObject(parent)
{
    

}

QString RobotBackend::test()
{
    return "C++ backend OK";
}

QString RobotBackend::connectSerival(QString device,int baudrate)
{
    connected_ = servo_.Feetech_Begin(device.toStdString(),baudrate);
    if(connected_){return "UART OK";}

    return "UART False";
}

QString RobotBackend::setTorque(int id,bool on)
{
    bool torque_status = servo_.Feetech_torque(id,on);

    if(connected_)
    {
        if(torque_status)
        {
            return "扭矩打开";
        }else
        {
            return "扭矩关闭";
        }
    }
    return "串口未连接";
}
QString RobotBackend::moveServo(int id,int position)
{
    if(!connected_)
    {
        return "串口未连接";
    }
    bool ok = servo_.Feetech_Safe_Move(id,position);

    if(ok)
    {
        return "Move OK";
    }
    return "Move False";
}

QString RobotBackend::ReadPos(int id)
{
    if(!connected_)
    {
        return "串口未连接";
    }
    int position = servo_.Feetech_ReadPos(id);
    return "当前位置:" + QString::number(position);
}

QString RobotBackend::startLearn()
{
    if(!connected_)
    {
        return "串口未连接";
    }
    if(learning_)
    {
        return "Already learning";
    }
    learning_ = true;
    return "Auto learn started";
}

QString RobotBackend::stopLearn()
{
    
    learning_ = false;
    return "Auto learn stoped";
}