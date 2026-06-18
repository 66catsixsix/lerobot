#pragma once
#include "serial.hpp"
#include <array>

class Feetech{
public:
    bool move(int id,int position);
    bool Feetech_Safe_Move(int id,int position);
    bool Feetech_Begin(const std::string &device,int baudrate);
    bool Feetech_torque(int id,bool status);
    bool Feetech_torque_off();
    bool Feetech_home(int id);
    bool Feetech_Move_Speed(int id,int position,int speed,int acc);
    bool Feetech_Safe_Move_Speed(int id, int position, int speed, int acc);
    bool Feetech_Soft_Stop(int id);
    bool Feetech_get_Elect(int id,int& electric);
    bool Feetech_get_Stress(int id,int& stress);
    int Feetech_ReadPos(int id);
    int Feetech_ReadTorque_status(int id);
    void scan();
    
    std::array<int, 7> home_Pos = {0,1978,860,3159,882,2036,1250};
    
private:
    LinuxSerial serial;
    int id;
    int position;
    int ID1_hard_min = 800;
    int ID1_hard_max = 3200;
    int ID2_hard_min = 1000;
    int ID2_hard_max = 3000;
    int ID3_hard_home = 3050;
    int ID3_hard_max = 1100;
    int ID4_hard_min = 1000;
    int ID4_hard_max = 2700;
    int ID5_hard_min = 350;
    int ID5_hard_max = 3750;
    int ID6_hard_min = 1300;
    int ID6_hard_max = 2400;
    
    //回位
    int ID1_home = 1978;
    int ID2_home = 860;
    int ID3_home = 3159;
    int ID4_home = 882;
    int ID5_home = 2036;
    int ID6_home = 1250;

    
};

