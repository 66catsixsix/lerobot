#include "feetech.hpp"
#include "learner.hpp"
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <ctime>
#include <sstream>
#include <iomanip>

Learner::Learner(Feetech& servo) : servo_(servo)
{
}

bool Learner::Low_Speed_Check(int id)
{   
    int current = servo_.Feetech_ReadPos(id);
    int target = 0;
    int after = 0;
    int startPos = hard_min[id] + 100;
    int endPos = hard_max[id] - 100;
    int Safe_Move_p = 15;
    bool startReached = false; //安全扫描起点

    if(current < startPos)
    {
        std::cout << "起点不在安全位置中，开始移动至安全点" << std::endl;
        bool SafeMove_Ok = servo_.Feetech_Safe_Move_Speed(id,startPos,300,20);
            if(SafeMove_Ok)
            {
                for(int i = 0;i < 60; i++)
            {
                if(stopRequested)
                {
                    servo_.Feetech_Soft_Stop(id);
                    return false;
                }

                current = servo_.Feetech_ReadPos(id);
                if(current >=0 && std::abs(current - startPos) <= Safe_Move_p)
                {
                    startReached = true;
                    break;
                }
                usleep(50000);
            }
            if(!startReached)
            {
                std::cout << "移动到扫描起点超时" << std::endl;
                return false;
            }
            std::cout << "已移动到安全点，继续检测" << std::endl;
        }
        if(!SafeMove_Ok)
        {
            std::cout << "移动至安全点失败,请检查" << std::endl;
            return false;
        }
    }
    else if(current > endPos)
    {
        std::cout << "终点不在安全位置中，开始移动至安全点" << std::endl;
        bool SafeMove_Ok = servo_.Feetech_Safe_Move_Speed(id,endPos,100,20);
        if(SafeMove_Ok)
        {
            for(int i = 0;i < 60;i++)
            {
                 if(stopRequested)
                {
                    servo_.Feetech_Soft_Stop(id);
                    return false;
                }

            current = servo_.Feetech_ReadPos(id);
            if(current >= 0 && std::abs(current - endPos) <= Safe_Move_p)
            {
                startReached = true;
                break;
            }
            usleep(50000);
            }
            if(!startReached)
            {
                std::cout << "移动到安全点超时" << std::endl;
                return false;
            }
            std::cout << "移动至安全点成功,检测重新开始" << std::endl;
        }
        if(!SafeMove_Ok)
        {
            std::cout << "安全点移动失败，请检查重试" << std::endl;
            return false;
        }
    }
    if(current > hard_min[id] + 50  &&  current < hard_max[id] - 50)
    {
        int step = 50;
        int speed = 100;
        int acc = 20;
        int direction = 1;
        bool goBack = false;
        //
        std::time_t now = std::time(nullptr);
        std::tm* t = std::localtime(&now);

        std::ostringstream filename;
        filename << "logs/learn_"
                 << std::put_time(t,"%Y%m%d_%H%M%S")
                 << ".csv";

        std::ofstream log(filename.str());

        if(!log.is_open())
        {
            std::cout << "日志文件打开失败" << std::endl;
            return false;
        }
        log << "id,target,after,direction,error,delta,stress,absStress,result," 
        << "pos1,pos2,pos3,pos4,pos5,pos6"
        << std::endl;

        //
        while (1)
        {
            if(stopRequested)
            {
                std::cout << "程序问题，启动急停" << std::endl;
                servo_.Feetech_Soft_Stop(id);
                return false;
            }
            current = servo_.Feetech_ReadPos(id);
            if(current < 0)
            {
                std::cout << "当前位置读取失败，停止检测" << std::endl;
                return false;
            }

            target = current + direction * step;
            
            if(target >= hard_max[id])
            {
                std::cout << "到达上边界，开始反向扫描" << std::endl;
                direction = -1;
                goBack = true;
                continue;
            }
            if(target <= hard_min[id])
            {
                if(goBack)
                {
                    std::cout << "往返扫描完成，检测结束" << std::endl;
                    return true;
                }

                direction = 1;
                continue;
            }
            std::cout << "LOW SPEED CHECK OK" << std::endl;
            std::cout << "开始低速度检测空间,ID" << id << std::endl;
            std::cout << "当前位置为:" << current << std::endl;
            std::cout << "目标位置为:" << target << std::endl;
            bool ok = servo_.Feetech_Safe_Move_Speed(id,target,speed,acc);
            if(!ok)
            {
                std::cout << "移动命令失败，停止检测" << std::endl;
                return false;
            }
            
            //var
            int tolerance = 10; //容差 判断是否到达。 error_limit为判断离谱范围
            int timeoutCount = 40;
            int error = 0;
            int error_limit = 80;

            int connect_times = 0;   //通信次数
            bool connect_error = false;  //通信错误
            int connect_error_limit = 5; //通信错误上限
            
            int lastPos = current; //上次位置
            int stuckCount = 0; //卡住的计数量
            int moveThreshold = 3; //变化量限制
            int stuckLimit = 8; //卡住上限次数
            bool blocked = false; //阻挡状态
            bool reached = false; //到达目标状态
            int delta = 0;
            //blocker  
            int stressStuckCount = 0;
            int stressStuckLimit = 5;
            int absStress = 0;
            int stressLimit = 300;
            int stress = 0;
            

            for(int i = 0; i < timeoutCount; i++)
            {
                if(stopRequested)
                {
                    servo_.Feetech_Soft_Stop(id);
                    std::cout << "收到停止请求，已执行软急停" << std::endl;
                    return false;
                }
            after = servo_.Feetech_ReadPos(id);
                if(after < 0)
                {
                    connect_times++;
                    if(connect_times == connect_error_limit)
                    {
                        connect_error = true;
                        break;
                    }
                    if(connect_times < connect_error_limit)
                    {
                        usleep(50000);
                        continue;
                    }
                    usleep(50000);
                    continue;
                }
                if(after >= 0)
                {
                    std::cout << "读取成功" << std::endl;
                    connect_times = 0;
                    error = std::abs(after - target);
                }
                if(error <= tolerance)
                {
                    reached = true;
                    break;
                }

                delta = std::abs(after - lastPos);
                lastPos = after;
                if(delta < moveThreshold && error >tolerance)
                {
                    stuckCount++;
                }else
                {
                    stuckCount = 0;
                }


                bool stressOk = servo_.Feetech_get_Stress(id,stress);
                if(stressOk)
                {
                    absStress = std::abs(stress);
                }
                else{
                    absStress = 0;
                }
                if(absStress > stressLimit && delta < moveThreshold && error > error_limit)
                {
                    stressStuckCount++;
                }
                else{
                    stressStuckCount = 0;
                }
               //BUG
                if(i > 2 && (stuckCount >= stuckLimit || stressStuckCount >= stressStuckLimit))
                {
                    blocked = true;
                    break;
                }
                usleep(50000);


            }

            if(connect_error == true)
            {
                servo_.Feetech_Soft_Stop(id);
                std::cout << "连续通信错误，检测停止" << std::endl;
                log << "result" << ","
                << id << "," << target << "," << after << "," << direction << "," << error
                << connect_error;
                return false;
            }
            if(!reached && !blocked)
            {
                std::cout << "等待到位超时，停止关节检测" << std::endl;
                log << id << "," << target << "," << after << ","
                << direction << "," << error  << "," << "timeout" << std::endl;
                
                return false;
            }
            //fixme 如果对应id舵机碰到阻挡则回退到安全点继续下一个舵机
             if(blocked)
            {
                std::array<int,7> position;
                for(int servoID = 1;servoID <= 6;servoID++)
                {
                    position[servoID] = servo_.Feetech_ReadPos(servoID);
                }
                std::cout << "检测到阻挡,记录阻挡ID点位" << std::endl;
                    log << id << ","
                        << target << ","
                        << after << ","
                        << direction << ","
                        << error << ","
                        << delta << ","
                        << stress << ","
                        << absStress << ","
                        << "blocked";
             
                for(int servoID = 1; servoID <= 6; servoID++)
                {
                    log << "," << position[servoID];
                }
                log << std::endl;

                int backTarget = after - direction * step * 2;

                bool backOk = servo_.Feetech_Safe_Move_Speed(id,backTarget,speed,acc);
                if(!backOk)
                {
                    std::cout << "安全回退检查失败" << std::endl;
                    return false;
                }
                usleep(1000000);
                return true;
            }
            
            if(error > error_limit)
            {
                std::cout << "误差过大,程序已停止" << std::endl;
                log << id << ","
                << target << ","
                << after << ","
                << direction << ","
                << error << ","
                << "error"
                << std::endl;

                return false;
            }
           
            std::cout << "移动完成后的位置为:" << after << std::endl;
            std::cout << "误差:" << error << std::endl;

            log << id << ","
                << target << ","
                << after << ","
                << direction << ","
                << error << ","
                << delta << ","
                << stress << ","
                << absStress << ","
                << "ok"
                << ",,,,,,"
                << std::endl;
        }
    }
    else
    {
        std::cout << "请检查空间大小及其空间位置!" << std::endl;
    }

}


bool Learner::Learner_All()
{
    bool homeReached = false;
    int homeTolerance = 15; //原点容差;
    int homePos = 0;
    for(int id = 1;id < 7;id++)
    {   
        if(stopRequested)
        {
            servo_.Feetech_Soft_Stop(id);
            std::cout << "全舵机学习错误,启动急停" << std::endl;
            resetStop();
            return false;
        }
        
        bool ok = Low_Speed_Check(id);
        homePos = servo_.home_Pos[id];
        if(!ok)
        {
            std::cout << "ID" << id << "自动检测失败，程序停止" << std::endl;
            return false;
        }
        bool homeOk = servo_.Feetech_home(id);
        homeReached = false;
        if(!homeOk)
        {
            std::cout << "ID" << id << "回位失败！" << std::endl;
            return false;
        }
        //focus
        //todo
        for(int i = 0;i < 680; i++)
        {
            int PosNow = servo_.Feetech_ReadPos(id);
            if(std::abs(PosNow - servo_.home_Pos[id]) <= homeTolerance)
            {
                homeReached = true;
                break;
            }
            if(stopRequested)
            {
                servo_.Feetech_Soft_Stop(id);
                return false;
            }
            usleep(50000);
        }
        if(homeReached == false)
        {
            std::cout << "回位超时" << std::endl;
            return false;
        }
        usleep(500000);
        std::cout << "空间检测完成" << std::endl;
    }

    return true;
}

void Learner::requestStop()
{
    stopRequested = true;
}

void Learner::resetStop()
{
    stopRequested = false;
}
