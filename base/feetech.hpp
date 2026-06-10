#pragma once
#include "serial.hpp"

class Feetech{
public:
    bool move(int id,int position);
    bool Feetech_Safe_Move(int id,int position);
    bool Feetech_Begin(const std::string &device,int baudrate);
    bool Feetech_torque(int id,bool status);
    int Feetech_ReadPos(int id);
    int Feetech_ReadTorque_status(int id);
    void scan();
    

private:
    LinuxSerial serial;
    int id;
    int position;
    int ID1_hard_min = 800;
    int ID1_hard_max = 3200;
    int ID2_hard_min = 1000;
    int ID2_hard_max = 3000;
    int ID3_home = 3050;
    int ID3_hard_max = 1100;
    int ID4_hard_min = 1000;
    int ID4_hard_max = 2700;
    int ID5_hard_min = 250;
    int ID5_hard_max = 3700;
    // int ID6_hard_min = ;
    // int ID6_hard_max = ;
    
};

