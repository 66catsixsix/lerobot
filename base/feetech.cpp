#include "feetech.hpp"
#include "serial.hpp"
#include <iostream>
#include <string>
#include <unistd.h>     // close, read, write
#include <fcntl.h>      // open, O_RDWR, O_NOCTTY, O_NDELAY
#include <termios.h>    // termios, tcgetattr, tcsetattr, baudrate
#include <cstring>  

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

bool Feetech::Feetech_ReadPos(int id)
{





}
