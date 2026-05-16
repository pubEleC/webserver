#pragma once

#include<iostream>
#include <time.h>
#include <chrono>

namespace myutils{

class TimeStamp
{
public:
    TimeStamp();
    explicit TimeStamp(int64_t microSecondsSinceEpoch_);

    static TimeStamp now();
    std::string toString() const;

    bool valid() const { return microSecondsSinceEpoch_ > 0; }

    // for internal usage.
    int64_t microSecondsSinceEpoch() const { return microSecondsSinceEpoch_; }
    time_t secondsSinceEpoch() const
    { return static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond); }


private:
    static const int kMicroSecondsPerSecond = 1000 * 1000;

    int64_t microSecondsSinceEpoch_;
    
};

}