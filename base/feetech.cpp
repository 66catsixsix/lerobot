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

    int read_re = serial.send(packet,8);
    if (read_re != 8)return false;

    uint8_t rx[16];
    int get_rx = serial.readBytes(rx,sizeof(rx),100);
    if(get_rx > 0)
    {   
        std::cout << "Packet:" << get_rx << std::endl;
        std::cout << "RX bytes:";
        for(int i = 0;i < get_rx;i++)
        {
            std::cout << std::hex << (int)rx[i];
        }
        std::cout << std::endl;
        int position = (rx[5])+(rx[6]<<8);
        std::cout << "舵机ID:" << id << std::endl;
        std::cout << std::dec <<  "位置:" << position << std::endl;

    }else if(get_rx == 0)
    {
        std::cout << "空包" << std::endl;
    }else if(get_rx != 8)
    {
        std::cout << "包长度错误" << std::endl;
        return 0;
    }
    else
    {
        std::cout << "接收信息错误" << std::endl;
        return 0;
    }
    return read_re == 8;

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
