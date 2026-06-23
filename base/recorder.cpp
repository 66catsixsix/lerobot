#include "recorder.hpp"
#include "serial.hpp"
#include "feetech.hpp"
#include <fstream>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <chrono> //time
#include <unistd.h>
#include <array>

Recorder::Recorder(Feetech& servo) : servo_(servo)
{
}

bool Recorder::parseRecordLine(const std::string& line,int& time_ms,std::array<int,7>&pos)
{
    std::stringstream ss(line);
    std::string item;

    if(!std::getline(ss,item,',')) //获取第一个字段  到逗号停止并且覆盖
    {
        return false;
    }

    time_ms = std::stoi(item);  
    for(int id = 1;id<=6;id++)
    {
        if(!std::getline(ss,item,','))
        {
            return false;
        }

        pos[id] = std::stoi(item);
    }
    return true;
}

bool Recorder::Way_Record(int seconds)
{   
    servo_.Feetech_torque_off();    //note 关闭全部扭矩

    
    std::time_t now = time(nullptr); //cat:获取当前时间
    std::tm* t = std::localtime(&now);//cat:转换年月日
    std::ostringstream filename; //cat:拼文件名
    filename << "logs/record_";
    filename << std::put_time(t, "%Y%m%d_%H%M%S");
    filename << ".csv";
    

    std::ofstream log(filename.str());
    if(!log.is_open())
    {
        std::cout << "日志打开失败" << std::endl;
        return false;
    }
    log << "time_ms,pos1,pos2,pos3,pos4,pos5,pos6,stress1,stress2,stress3,stress4,stress5,stress6" << std::endl;

    auto start = std::chrono::steady_clock::now();
    while (true)
    {
        auto now = std::chrono::steady_clock::now();

        long long time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
        
        if(time_ms >= seconds * 1000)
        {
            break;
        }
        int pos1 = servo_.Feetech_ReadPos(1);
        int pos2 = servo_.Feetech_ReadPos(2);
        int pos3 = servo_.Feetech_ReadPos(3);
        int pos4 = servo_.Feetech_ReadPos(4);
        int pos5 = servo_.Feetech_ReadPos(5);
        int pos6 = servo_.Feetech_ReadPos(6);

        int stress1 = 0;
        int stress2 = 0;
        int stress3 = 0;
        int stress4 = 0;
        int stress5 = 0;
        int stress6 = 0;

        servo_.Feetech_get_Stress(1,stress1);
        servo_.Feetech_get_Stress(2,stress2);
        servo_.Feetech_get_Stress(3,stress3);
        servo_.Feetech_get_Stress(4,stress4);
        servo_.Feetech_get_Stress(5,stress5);
        servo_.Feetech_get_Stress(6,stress6);

        log << time_ms << ","
            << pos1 << "," << pos2 << "," << pos3 << "," << pos4 << "," 
            << pos5 << "," << pos6 << "," << stress1 << "," << stress2
            << "," << stress3 << "," << stress4 << "," << stress5 << ","
            << stress6 << std::endl;
        
        usleep(50000);
    
    }
    
    std::cout << "路径记录完成" << std::endl;
    return true;
    
}


bool Recorder::Way_Replay(const std::string& filename ,int id)
{
    std::ifstream file(filename); //cat 打开文件
    if (!file.is_open())
    {
        std::cout << "路径文件打开失败"  << std::endl;
        return false;
    }

    std::string line;
    std::getline(file,line);  //读表头  执行第一次去掉表头

    if(std::getline(file,line))
    {
        std::stringstream ss(line);    //头
        std::string item;   
       

        std::getline(ss,item,',');
        int time_ms = std::stoi(item);  

        std::getline(ss,item,',');
        int pos1 = std::stoi(item); //pos1
 
        std::getline(ss,item,',');
        int pos2 = std::stoi(item); //pos2

        std::getline(ss,item,',');
        int pos3 = std::stoi(item); //pos3

        std::getline(ss,item,',');
        int pos4 = std::stoi(item); //pos4

        std::getline(ss,item,',');
        int pos5 = std::stoi(item); //pos5

         std::getline(ss,item,',');
        int pos6 = std::stoi(item); //pos6

        std::array<int,7> pos = {0,pos1,pos2,pos3,pos4,pos5,pos6};
        int start_point = pos[id];
    
        std::cout << "移动到开始起点" << std::endl;
        servo_.Feetech_Move_Speed(id,start_point,1000,50);
        usleep(2000000);
        std::cout << "开始进程" << std::endl;
    }
    int last_time = 0;
    while(std::getline(file,line))
    {
        // std::cout << line << ",";//读行数据
        // std::cout << std::endl;

        std::stringstream ss(line);  
        std::string item;
        getline(ss,item,',');
        int time_ms = std::stoi(item);
        
        getline(ss,item,',');
        int pos1 = std::stoi(item);

        getline(ss,item,',');
        int pos2 = std::stoi(item);

        getline(ss,item,',');
        int pos3 = std::stoi(item);

        getline(ss,item,',');
        int pos4 = std::stoi(item);

        getline(ss,item,',');
        int pos5 = std::stoi(item);
        
        getline(ss,item,',');
        int pos6 = std::stoi(item);

        if(id < 1 || id > 6)
        {
            std::cout << "ID错误"  << std::endl;
            return false;
        }
        std::array<int,7> pos = {0,pos1,pos2,pos3,pos4,pos5,pos6};
        int target = pos[id];
        std::cout << time_ms << " "
                 << pos1 << " "
                 << pos2 << " "
                 << pos3 << " "
                 << pos4 << " "
                 << pos5 << " "
                 << pos6 << " " 
                 << std::endl;
        int delay_ms = time_ms - last_time;
        last_time = time_ms;
        servo_.Feetech_Move_Speed(id,target,1800,70);
        usleep(delay_ms * 1000);
        int real = servo_.Feetech_ReadPos(id);
        int error = std::abs(real - target);
        if(error > 150){
            std:: cout << "舵机" << id << "路径滞后！" << "Tips:" << error << std::endl;
        }
        //动态速度
    }
    std::cout << "路径读取完成" << std::endl;
    return true;
}

bool Recorder::Way_Replay_ALL(const std::string& filename)
{
    std::ifstream file(filename); //cat 打开文件
    if (!file.is_open())
    {
        std::cout << "路径文件打开失败"  << std::endl;
        return false;
    }

        std::cout << "全部回到home点" << std::endl;
        servo_.Feetech_home_ALL();
        std::cout << "全部舵机replay开始进程" << std::endl;
        usleep(2000000);

        std::string line;          //创建字符串变量存取一行文本
        std::getline(file,line);  //从file读第一行(表头)放入line执行第一次去掉表头times pos1 pos2..


    int last_time = 0;
    while(std::getline(file,line))
    {

            int time_ms = 0;
            std::array <int,7> pos = {};

            bool ok = parseRecordLine(line,time_ms,pos);

            if(!ok)
            {
                std::cout << "路径行解析失败" << std::endl;
                continue;
            }

        std::cout << time_ms << " "
                 << pos[1] << " "
                 << pos[2] << " "
                 << pos[3] << " "
                 << pos[4] << " "
                 << pos[5] << " "
                 << pos[6] << " " 
                 << std::endl;

        for(int id = 1;id <= 6;id++)
        {
            servo_.Feetech_Move_Speed(id,pos[id],1800,60);
        }
        int delay_ms = time_ms - last_time;
        last_time = time_ms;
        usleep(delay_ms * 1000);
        
       for(int id = 1;id <= 6;id++)
       {
        int real = servo_.Feetech_ReadPos(id);
        int error = std::abs(real - pos[id]);
        
        if(error > 150)
        {
            std::cout << "舵机滞后" << error << std::endl;
        }
       }
        //动态速度
    }
    std::cout << "路径读取完成" << std::endl;
    return true;

}

