#include <iostream>
#include "SCServo.h"
#include "serial.hpp"
#include <cstring>
#include "feetech.hpp"
#include "SMS_STS.h"

Feetech servo;

int main(int argc,char* argv[]){
    if(argc == 1)
    {
        std::cout << "参数错误：参数不足" << std::endl;
        return 0;
    }else if(argc == 2)
    {
        std::cout << "参数错误：参数不足" << std::endl;
        return 0;
    }
    bool ef = servo.Feetech_Begin("/dev/ttyACM1",1000000); //RK
    //bool ef = servo.Feetech_Begin("/dev/ttys003",115200);
    std::cout << std::boolalpha;
    if(ef)
    {
        std::cout << "UART OK" << std::endl;
    }else
    {
        std::cout << "UART False" << std::endl;
    }

    std::string cmd = argv[1];
    if(cmd == "move" && argc == 4){
        int id = 0;
        int position = 0;
        try{
            id = std::stoi(argv[2]);
            position = std::stoi(argv[3]);
        }
        catch(...){
            std::cout << "(参数错误!)请输入正确的字符" << std::endl;
            return 0;
        }

        int torque_status = servo.Feetech_ReadTorque_status(id);
        bool move_ok;
        if(torque_status == 1)
        {
            move_ok = servo.move(id,position);
        }else if(torque_status == 0)
        {
            std::cout << "扭矩未启用,启用扭矩后再试。" << std::endl;
            return 0;
        }
        if(move_ok)
        {
            std::cout << "MOVE OK" << std::endl;
        }else
        {
            std::cout << "MOVE False" << std::endl;
        }
    }
    
    else if(cmd == "torque" && argc == 4){
        
        int id;
        std::string cmd = argv[3];
        int status;
        try{
            id = std::stoi(argv[2]);
        }catch(...){

            std::cout << "ID错误 请输入正确ID" << std::endl;
            return 0;
        }
        
        if(cmd == "on" || cmd == "enable")
        {
            status = 0x01;
            std::cout << "扭矩已打开";

        }else if(cmd == "off" || cmd == "disable")
        {
            status = 0x00;
            std::cout << "扭矩已关闭";
        }else
        {
            std::cout << "(参数错误)enbale 或者 disable";
            return 0;
        }
        servo.Feetech_torque(id,status);

    }
    else if(cmd == "read" && argc == 3)
    {
        int id = 0;
        try{
            id = std::stoi(argv[2]); 
        }catch(...){
            std::cout << "ID输入错误,请重试" << std::endl;
            return 0;
        }
        int read_ok = servo.Feetech_ReadPos(id);
        std::cout << read_ok << std::endl; 
    }
    else if(cmd == "readtorque" && argc == 3)
    {   int id;
        try{
        id = std::stoi(argv[2]);
        }
        catch(...)
        {
            std::cout << "ID错误!" << std::endl;
            return 0;
        }
        servo.Feetech_ReadTorque_status(id);
    }
    else{
        std::cout << "参数填写错误，请重新输入" << std::endl;
    }
    

    if (cmd == "officialread") {
    SMS_STS st;
    st.begin(1000000, "/dev/ttyACM1");

    int id = std::stoi(argv[2]);
    int pos = st.ReadPos(id);

    std::cout << "official pos: " << pos << std::endl;
    return 0;
}
  


}
