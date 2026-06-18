#include "feetech.hpp"
#include "serial.hpp"
#include <iostream>
#include <string>
#include <unistd.h>     // close, read, write
#include <fcntl.h>      // open, O_RDWR, O_NOCTTY, O_NDELAY
#include <termios.h>    // termios, tcgetattr, tcsetattr, baudrate
#include <cstring>  
#include <iomanip>
#include <algorithm>

bool Feetech :: Feetech_Begin(const std::string &device,int baudrate){

   return serial.openPort(device,baudrate);

}

bool Feetech::move(int id,int position)
{   
    if(id<1 || id>253)
    {
        std::cout << "ID不在区间内!" << std::endl;
        return 0;
    }
    if(position<0 || position>4095)
    {
        std::cout << "位置超过限制步数!" << std::endl;
        return 0;
    }
    uint8_t packet[9];
    int length = 0x05;
    int instruction = 0x03;
    int address = 0x2A;
    uint8_t poslow = position & 0xFF;
    uint8_t posHigh = (position >> 8) & 0xFF; //整体向右移动8个二进制位
    uint8_t checknum = ~(id+length+instruction+address+poslow+posHigh);

    packet[0] = 0xFF;
    packet[1] = 0xFF;
    packet[2] = id;
    packet[3] = length;
    packet[4] = instruction;
    packet[5] = address;
    packet[6] = poslow;
    packet[7] = posHigh;
    packet[8] = checknum;

    std::cout << "舵机ID:" << (int)packet[2] << std::endl;
    std::cout << "长度:" << (int)packet[3] << std::endl;
    std::cout << "指令:" << (int)packet[4] << std::endl;
    std::cout << "地址:" << (int)packet[5] << std::endl;
    std::cout << "低位:" << (int)packet[6] << std::endl;
    std::cout << "高位:" << (int)packet[7] << std::endl;
    std::cout << "校验和:" << (int)packet[8] << std::endl;
    std::cout << std::endl;

    
    int written = serial.send(packet,9);
    return written == 9;

}


bool Feetech :: Feetech_torque(int id,bool status)
{
    int length = 0x04;
    int instruction = 0x03;
    int torque = 0x28;
    bool enable = 0x01;
    bool disable = 0x00;
    uint8_t checksum = ~(id+length+instruction+torque+status);
    uint8_t packet[8];
    packet[0] = 0xFF;
    packet[1] = 0xFF;
    packet[2] = id;
    packet[3] = length;
    packet[4] = instruction;
    packet[5] = torque;
    packet[6] = status;
    packet[7] = checksum;

    std::cout << packet[2] << std::endl;
    std::cout << packet[3] << std::endl;
    std::cout << packet[4] << std::endl;

    int status_scan = serial.send(packet,8);
    return status_scan == 8;

} 

bool Feetech::Feetech_torque_off()
{
    Feetech_torque(1,0);
    usleep(300000);
    Feetech_torque(2,0);
    usleep(300000);
    Feetech_torque(3,0);
    usleep(300000);
    Feetech_torque(4,0);
    usleep(300000);
    Feetech_torque(5,0);
    usleep(300000);
    Feetech_torque(6,0);
    usleep(300000);
    std::cout << "扭矩已经全部关闭" << std::endl;
    return 0;
}

int Feetech::Feetech_ReadPos(int id)
{
    int length = 0x04;
    int instruction = 0x02;
    uint8_t packet[8];
    uint8_t checknum = ~(id + length + instruction + 0x38 + 0x02);
    packet[0] = 0xFF;
    packet[1] = 0xFF;
    packet[2] = id;
    packet[3] = length;
    packet[4] = instruction;
    packet[5] = 0x38;
    packet[6] = 0x02;
    packet[7] = checknum; 
    serial.flushInput();
    int read_re = serial.send(packet,8);
    if (read_re != 8)return -1;

    // uint8_t rx[16];
    // int get_rx = serial.readBytes(rx,sizeof(rx),100);
    // if(get_rx > 0)
    // {   
    //     std::cout << "Packet:" << get_rx << std::endl;
    //     std::cout << "RX bytes:";
    //     for(int i = 0;i < get_rx;i++)
    //     {
    //         //std::cout << std::hex << (int)rx[i];
    //         std::cout << std::hex << (int)rx[i] << " ";
    //     }
    //     std::cout << std::dec << std::endl;
    //     int position = (rx[5])+(rx[6]<<8);
    //     std::cout << "舵机ID:" << id << std::endl;
    //     std::cout << std::dec <<  "位置:" << position << std::endl;
    //     return position;

    // if(get_rx == 0)
    // {
    //     std::cout << "空包" << std::endl;
    // }else if(get_rx != 8)
    // {
    //     std::cout << "包长度错误" << std::endl;
    //     return 0;
    // }
    // else
    // {
    //     std::cout << "接收信息错误" << std::endl;
    //     return 0;
    // }
    // return -1;

           uint8_t rx[8];
           int total = 0;

           while (total < 8)
           {
            int n = serial.readBytes(rx + total,8 - total,100);

            if(n<=0){
                break;
            }
            total += n;

           }
           
           if(total != 8)
           {
            std::cout << "包长度错误:" << total <<std::endl;
            return -1;
           }
           if(rx[0] != 0xFF || rx[1] != 0xFF)
           {
            std::cout << "包头错误" << std::endl;
            return -1;
           }
           if(rx[2] != id)
           {
            std::cout << "ID错误" <<std::endl;
            return -1;
           }
           if(rx[3] != 0x04)
           {
            std::cout << "长度字段错误" << std::endl;
            return -1;
           }
           if(rx[4] != 0x00)
           {
            std::cout << "返回错误码:" << (int)rx[4] << std::endl;
            return -1;
           }
           
           uint8_t checksum = ~(rx[2] + rx[3] + rx[4] + rx[5] + rx[6]);
           if(checksum != rx[7])
           {
            std::cout << int(rx[0]) << std::endl;
            std::cout << int(rx[1]) << std::endl;
            std::cout << int(rx[2]) << std::endl;
            std::cout << int(rx[3]) << std::endl;
            std::cout << int(rx[4]) << std::endl;
            std::cout << int(rx[5]) << std::endl;
            std::cout << int(rx[6]) << std::endl;
            std::cout << int(rx[7]) << std::endl;
            
            std::cout << "校验和错误" << std::endl;
            return -1;
           }

           int position = rx[5] + (rx[6] << 8);
           std::cout << "舵机ID:" << id << std::endl;
           std::cout << "位置:" << position <<std::endl;

           return position;
        }

int Feetech::Feetech_ReadTorque_status(int id)
{
    int length = 0x04;
    int instruction = 0x02;
    int torque = 0x28;
    int readone = 0x01;
    int getone;
    uint8_t checksum = ~(id+length+instruction+torque+readone);
    uint8_t packet[8];
    packet[0] = 0xFF;
    packet[1] = 0xFF;
    packet[2] = id;
    packet[3] = length;
    packet[4] = instruction;
    packet[5] = torque;
    packet[6] = 0x01;
    packet[7] = checksum;

    uint8_t rx[8];
    int writen_len = serial.send(packet,8);
    if(writen_len != 8)
    {
        std::cout << "包发送失败" << std::endl;
        return 0;
    }
    int get_rx = serial.readBytes(rx,sizeof(rx),100);
    int get_status = rx[5];
    if(get_status == 1){
        std::cout << "扭矩已打开" << std::endl;
        return get_status; 

    }else if(get_status == 0)
    {
        std::cout << "扭矩已关闭" << std::endl;
        return get_status;
    }

}

bool Feetech::Feetech_Safe_Move(int id,int position)
{   
    switch (id)
    {
    case 1: 
        if(position<ID1_hard_min || position > ID1_hard_max)
        {
            std::cout << "位置超过安全限制,请关闭电源!" << std::endl;
            return false;
        }
            return move(id,position);

    case 2:
        if(position<ID2_hard_min || position > ID2_hard_max)
        {
            std::cout << "位置超过安全限制,请关闭电源!" << std::endl;
            return false;
        }
            return move(id,position);
    
    case 3:
        if(position<ID3_hard_max || position > ID3_home)
        {
            std::cout << "位置超过安全限制,请关闭电源!" << std::endl;
            return false;
        }
            return move(id,position);
    
    case 4:
        if(position<ID4_hard_min || position > ID4_hard_max)
        {
            std::cout << "位置超过安全限制,请关闭电源!" << std::endl;
            return false;
        }
            return move(id,position);
    
    case 5:
        if(position<ID5_hard_min || position > ID5_hard_max)
        {
            std::cout << "位置超过安全限制,请关闭电源!" << std::endl;
            return false;
        }
            return move(id,position);
    
    case 6:
        if(position<ID6_hard_min || position > ID6_hard_max)
        {
            std::cout << "位置超过安全限制,请关闭电源!" << std::endl;
            return false;
        }
            return move(id,position);

    default:
            return false;
            
    } 
}

bool Feetech::Feetech_home(int id)
{
    switch (id)
    {
    case 1:
        return Feetech_Move_Speed(id,ID1_home,1000,50);
    case 2:
        return Feetech_Move_Speed(id,ID2_home,1000,50);
    case 3:
        return Feetech_Move_Speed(id,ID3_home,1000,50);
    case 4:
        return Feetech_Move_Speed(id,ID4_home,1000,50);
    case 5:
        return Feetech_Move_Speed(id,ID5_home,1000,50);
    case 6:
        return Feetech_Move_Speed(id,ID6_home,1000,50);

    default:
        return false;
    }
}

bool Feetech::Feetech_Move_Speed(int id,int position,int speed,int acc)
{
    int length = 0x0A;
    int instruction = 0x03;
    int addr = 0x29;
    uint8_t ACC = acc & 0xFF;
    uint8_t POS_Low = position & 0xFF;
    uint8_t POS_High = (position >> 8) & 0xFF;
    int TIME_Low = 0x00;
    int TIME_High = 0x00;
    uint8_t SPEED_Low = speed & 0xFF;
    uint8_t SPEED_High = (speed >> 8) & 0xFF;
    uint8_t checksum = ~(id + length + instruction + addr + ACC + POS_Low + POS_High + 0 + 0 + SPEED_Low + SPEED_High);
    uint8_t packet[14];
    packet[0] = 0xFF;
    packet[1] = 0xFF;
    packet[2] = id;
    packet[3] = length;
    packet[4] = instruction;
    packet[5] = addr;
    packet[6] = ACC;
    packet[7] = POS_Low;
    packet[8] = POS_High;
    packet[9] = TIME_Low;
    packet[10] = TIME_High;
    packet[11] = SPEED_Low;
    packet[12] = SPEED_High;
    packet[13] = checksum;

    int written = serial.send(packet,14);
    return written == 14;
}

//
bool Feetech::Feetech_Safe_Move_Speed(int id,int position,int speed,int acc)
{   
    speed = std::clamp(speed,0,3400);
    acc = std::clamp(acc,0,254);
    
    //id3 1100和3050 
    std::array<int,7> hard_min = {0,800,1000,1100,1000,350,1300};
    std::array<int,7> hard_max = {0,3200,3000,3050,2700,3750,2400};
    

    if(id < 1 || id > 6){std::cout << "ID有误!" << std::endl;return false;}
    if(position < hard_min[id] || position > hard_max[id]){std::cout << "位置超出安全限制" << std::endl; return false;}

    return Feetech_Move_Speed(id,position,speed,acc);

}

bool Feetech::Feetech_Soft_Stop(int id)
{
    int current = Feetech_ReadPos(id);

    if(current < 0)
    {
        return false;
    }
    
    return Feetech_Move_Speed(id,current,150,20);

}

bool Feetech::Feetech_get_Elect(int id,int& electric)
{
    int length = 0x04;
    int instruction = 0x02;
    uint8_t POS_Low_E = 0x45;
    uint8_t addr = 0x02;
    uint8_t checksum = ~(id+ length + instruction + POS_Low_E + addr);

    uint8_t packet[8];
    packet[0] = 0xFF;
    packet[1] = 0xFF;
    packet[2] = id;
    packet[3] = length;
    packet[4] = instruction;
    packet[5] = POS_Low_E;      //todo   起始位置
    packet[6] = addr;           //fixme  长度
    packet[7] = checksum;
    
    serial.flushInput();
    int written = serial.send(packet,8);
    if(written != 8)
    {
        std::cout << "发送电流读取包失败" << std::endl;
        return false;
    }
    //var. s aso
    uint8_t rx[8];
    int total = 0;

    while (total < 8)
    {   
        int n = serial.readBytes(rx + total, 8 - total,100);
    
        if(n<=0)
        {
            break;
        }
        total += n;
    }
      if (total != 8)
        {
            std::cout << "包长度错误" << std::endl;
            return false;
        }
        if(rx[0] != 0xFF || rx[1] != 0xFF)
        {
            std::cout << "包头错误" << std::endl;
            return false;
        }
        if(rx[2] != id)
        {
            std::cout << "ID错误" << std::endl;
            return false;
        }
        if(rx[3] != length)
        {
            std::cout << "包长度错误" << std::endl;
            return false;
        }
        if(rx[4] != 0x00)
        {
            std::cout << "指令码错误:" << (int)rx[4] << std::endl;
            return false;
        }
        uint8_t rxchecksum = ~(rx[2] + rx[3] + rx[4] + rx[5] + rx[6]);
        if(rx[7] != rxchecksum)
        {
            std::cout << "校验和错误" << std::endl;
            return false;
        }
        //15位方向
        int raw = (int)rx[5] + ((int)rx[6] << 8);
        //拿到正负清除15位 然后得到电流大小
        electric = (raw & 0x8000)? -(raw & 0x7FFF) : raw;
        std::cout << "舵机" << id << "电流为:" << electric << std::endl;
        return true;

}

bool Feetech::Feetech_get_Stress(int id,int& stress)
{
    
    uint8_t length = 0x04;
    uint8_t instruction = 0x02;
    uint8_t addr = 0x02;
    uint8_t POS_Low_S = 0x3C;
    uint8_t readLen = 0x02;
    uint8_t checksum = ~(id + length + instruction + POS_Low_S + readLen);

    uint8_t packet[8];
    packet[0] = 0xFF;
    packet[1] = 0xFF;
    packet[2] = id;
    packet[3] = length;
    packet[4] = instruction;
    packet[5] = POS_Low_S;
    packet[6] = readLen;
    packet[7] = checksum;
    serial.flushInput();
    int written = serial.send(packet,sizeof(packet));
    if(written != 8)
    {
        std::cout << "负载包发送失败" << std::endl;
        return false;
    }


    uint8_t rx[8];
    int total = 0;

    while(total < 8)
    {
        int get_stress = serial.readBytes(rx+total,8-total,100);
        if(get_stress <= 0)
        {
            break;
        }
        total += get_stress;
    }
    if(total != 8)
    {
        std::cout << "包长度错误" << std::endl;
        return false;
    }
    if(rx[0] != 0xFF || rx[1] != 0xFF)
    {
        std::cout << "包头错误" << std::endl;
        return false;
    }
    if(rx[2] != id)
    {
        std::cout << "ID错误" << std::endl;
        return false;
    }
    if(rx[3] != length)
    {
        std::cout << "包长度错误" << std::endl;
        return false;
    }
    if(rx[4] != 0x00)
    {
        std::cout << "包指令错误" << std::endl;
        return false;
    }
    uint8_t rxchecksum = ~(rx[2] + rx[3] + rx[4] + rx[5] + rx[6]);
    if(rx[7] != rxchecksum)
    {
        std::cout << "校验和错误" << std::endl;
        return false;
    }

    int raw = (int)rx[5] | (rx[6] << 8);

    //blocker  如果0x400  相与十位结果不为0，就相0x7FFF的值，获得负载；
    stress = (raw & 0x400) ? -(raw & 0x03FF) : (raw & 0x03FF);
    std::cout << "舵机" << id << "的负载为:" << stress << std::endl;
    return true;
}
