#pragma once
#include <string>
class LinuxSerial{

public:
    bool openPort(const std::string& device,int baudrate);
    int writeBytes(const uint8_t* data,size_t length);
    int readBytes(uint8_t* buffer,size_t length,int timeoutMS);
    void closePort();
    bool isOpen() const;
    int send(const uint8_t* data, size_t length);

private:
    int fd_ = -1;

};
