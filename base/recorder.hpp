#pragma once
#include <iostream>
#include "feetech.hpp"
class Recorder{
public:
    explicit Recorder(Feetech& servo);
    bool Way_Record(int seconds);    //tip:路径记录.  单位秒
    bool Way_Replay(const std::string& filename ,int id);  //tips:路径复刻
    bool Way_Replay_ALL(const std::string& filename);
    bool parseRecordLine(const std::string& line,int& time_ms,std::array<int,7>&pos);

private:
    Feetech& servo_;
};