#include <iostream>
#include "serial.hpp"
#include <string>
#include <iostream>
#include <unistd.h>     // close, read, write
#include <fcntl.h>      // open, O_RDWR, O_NOCTTY, O_NDELAY
#include <termios.h>    // termios, tcgetattr, tcsetattr, baudrate
#include <cstring>      // memset


bool LinuxSerial::openPort(const std::string& device, int baudrate)
{
    fd_ = open(device.c_str(),O_RDWR | O_NOCTTY | O_NDELAY);

    if(fd_ < 0)
    {
        return false;
    }

    struct termios option;

    if(tcgetattr(fd_,&option) != 0)
    {
        close(fd_);
        return false;
    }

    speed_t speed;

    switch(baudrate)
    {
        case 9600:speed = B9600;break;
        case 115200:speed = B115200;break;
        #ifdef B1000000
        
        case 1000000:
        speed = B1000000;
        break;
        #endif        
        
        default:close(fd_);
                return false;

    }
        cfsetispeed(&option,speed);
        cfsetospeed(&option,speed);

        option.c_cflag |= (CLOCAL | CREAD); //打开收发 本地串口模式
        option.c_cflag &= ~PARENB;//校验位开关 不做奇偶校验
        option.c_cflag &= ~CSTOPB;//stop bit关闭两个停止位
        option.c_cflag &= ~CSIZE;
        option.c_cflag |= CS8;
        option.c_cflag &= ~CRTSCTS;

        if(tcsetattr(fd_,TCSANOW,&option) != 0)
        {
            close(fd_);
            return false;
        }
        

        return true;

}  

int LinuxSerial::send(const uint8_t* data, size_t length)
{
    return write(fd_,data,length);
}

int LinuxSerial::readBytes(uint8_t* buffer, size_t length, int timeoutMS)
{
    return read(fd_,buffer,length);
}
