#pragma once
#include "serial.hpp"

class Feetech{
public:
    bool move(int id,int position);
    bool Feetech_Begin(const std::string &device,int baudrate);
    bool Feetech_torque(int id,bool status);
    int Feetech_ReadPos(int id);
    int Feetech_ReadTorque_status(int id);
    void scan();
    

private:
    LinuxSerial serial;
    int id;
    int position;
    
};

