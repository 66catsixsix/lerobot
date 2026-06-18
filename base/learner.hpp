#pragma once
#include "feetech.hpp"
#include <algorithm>
#include <array>

class Learner{
    
public:
    Learner(Feetech& servo);
    bool Low_Speed_Check(int id);
    bool Learner_All();
    void requestStop();
    void resetStop();

    private:
    Feetech& servo_;
    //id3 1100和3050 
    std::array<int,7> hard_min = {0,800,1000,1100,1000,350,1300};
    std::array<int,7> hard_max = {0,3200,3000,3050,2700,3750,2400};
    bool stopRequested = false;



};