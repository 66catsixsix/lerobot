#include <iostream>
#include "SCServo.h"
#include "serial.hpp"
#include <cstring>
#include "feetech.hpp"
#include "SMS_STS.h"
#include "learner.hpp"
#include <csignal>

Feetech servo;
Learner learn(servo);

void handleSignal(int signal)
{
    if(signal == SIGINT)
    {
        learn.requestStop();
    }
}

int main(int argc,char* argv[]){

    std::signal(SIGINT,handleSignal); //ctrl c

    if(argc == 1)
    {
        std::cout << "参数错误：参数不足" << std::endl;
        return 0;
    }else if(argc == 2)
    {
        std::cout << "参数错误：参数不足" << std::endl;
        return 0;
    }
    bool ef = servo.Feetech_Begin("/dev/ttyACM0",1000000); //RK
    //bool ef = servo.Feetech_Begin("/dev/ttys003",115200);
    std::cout << std::boolalpha;
    if(ef)
    {
        std::cout << "UART OK" << std::endl;
    }else
    {
        std::cout << "UART False" << std::endl;
        return 0;
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
        bool move_ok = false;
        if(torque_status == 1)
        {
            move_ok = servo.Feetech_Safe_Move(id,position);
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
    else if(cmd == "torque" && argc == 3)
    {
        std::string target = argv[2];
        if(target == "off")
        {
            servo.Feetech_torque_off();
        }
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
    
    else if(cmd == "home" && argc == 3)
    {
        std::string target = argv[2];
         
        if(target == "all")
        {   
            servo.Feetech_home(1);
            usleep(300000);
            servo.Feetech_home(2);
            usleep(300000);
            servo.Feetech_home(3);
            usleep(300000);
            servo.Feetech_home(4);
            usleep(300000);
            servo.Feetech_home(5);
            usleep(300000);
            servo.Feetech_home(6);
            usleep(300000);
            
            std::cout << "HOME ALL OK" << std::endl;
            return 0;
        }

        int id = 0;
        try{
            id = std::stoi(argv[2]);
        }catch(...)
        {
            std::cout << "ID输入错误,请重新输入" << std::endl;
            return 0;
        } 
        bool ok = servo.Feetech_home(id);

        if(ok){
            std::cout << "HOME OK" << std::endl;
        }else{
            std::cout << "HOME False" << std::endl;
        }
    }

    else if (cmd == "officialread" && argc == 3) {
            SMS_STS st;
            st.begin(1000000, "/dev/ttyACM1");

            int id = std::stoi(argv[2]);
            int pos = st.ReadPos(id);

            std::cout << "official pos: " << pos << std::endl;
            return 0;
    }

    else if(cmd == "movespeed" && argc == 6)
    {
            int id = 0;
            int position = 0;
            int speed = 0;
            int acc = 0;
            
            try{
                id = std::stoi(argv[2]);
                position = std::stoi(argv[3]);
                speed = std::stoi(argv[4]);
                acc = std::stoi(argv[5]);
            }catch(...)
            {
                std::cout << "参数输入有误，请检查" << std::endl;
                return -1;
            }

            bool ok = servo.Feetech_Move_Speed(id,position,speed,acc);
            
            if(ok)
            {
                std::cout << "start move speed" << std::endl;
                std::cout << "ID" << id << std::endl;
                std::cout << "位置" << position << std::endl;
                std::cout << "速度" << speed << std::endl;
                std::cout << "加速度" << acc << std::endl;
                return 0;
            }
            if(!ok)
            {
                std::cout << "Speed Move False!" << std::endl;
                return -1;
            }
    }
    else if(cmd == "safemovespeed" && argc == 6)
    {
            int id = 0;
            int position = 0;
            int speed = 0;
            int acc = 0;

            try{
                id = std::stoi(argv[2]);
                position = std::stoi(argv[3]);
                speed = std::stoi(argv[4]);
                acc = std::stoi(argv[5]);
            }catch(...)
            {
                std::cout << "参数输入有误，请检查" << std::endl;
                return -1;
            }
            bool ok = servo.Feetech_Safe_Move_Speed(id,position,speed,acc);
            
            if(ok)
            {
                std::cout << "start safemove speed" << std::endl;
                std::cout << "ID" << id << std::endl;
                std::cout << "位置" << position << std::endl;
                std::cout << "速度" << speed << std::endl;
                std::cout << "加速度" << acc << std::endl;
                return 0;
            }
            if(!ok)
            {
                std::cout << "Speed Move False!" << std::endl;
                return -1;
            }

    }
    else if(cmd == "spacelearner" && argc == 3)
    {
 
        std::string target = argv[2];
        if(target == "all")
        {
            learn.resetStop();
            learn.Learner_All();
            return 0;
        }
        int id = 0;
        try{
            id = std::stoi(argv[2]);
        }
        catch(...)
        {
            std::cout << "ID解析错误,请检查输入" << std::endl;
            return false;
        }
        learn.resetStop();
        learn.Low_Speed_Check(id);
  
    }
    
    //bug 电流
    else if(cmd == "getelect" && argc == 3)
    {
        int id = 0;
        try
        {
            id = std::stoi(argv[2]);

        }catch(...)
        {
            std::cout << "ID解析错误" << std::endl;
            return -1;
        }
        
        int get_Elect = 0;
        bool ok = servo.Feetech_get_Elect(id,get_Elect);
        if(!ok)
        {
            std::cout << "电流异常，请检查" << std::endl;
            return -1;
        }
        std::cout << "舵机电流为:" << get_Elect << std::endl;
    }
    //bug 

    else if(cmd == "getstress" && argc == 3)
    {
        int id = 0;
        int get_Stress = 0;
        try{
            id = std::stoi(argv[2]);
        }
        catch(...)
        {
            std::cout << "ID解析错误" << std::endl;
        }
        bool ok = servo.Feetech_get_Stress(id,get_Stress);
        if(!ok)
        {
            std::cout << "负载获取失败，请检查" << std::endl;
            return -1;
        }
    }
 
    
       
    else{
        std::cout << "参数填写错误，请重新输入" << std::endl;
        return false;
    }
}
