#pragma once
#include <string>
#include <chrono>
#include <time.h>
#include <iomanip> // get_time
#include <sstream>

enum class timeType: char{us,ms,s};

inline int64_t get_now_time(timeType type){
    // time_t ti = time(NULL);
    // return TimeStamp(ti);
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    int64_t value = 0;
    switch (type)
    {
    case timeType::us:
        value = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
        break;
    case timeType::ms:
        value = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        break;
    case timeType::s:
        value = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
        break;    
    default:
        break;
    }
    return value;
}

//请输入us
inline std::string time_to_String(int64_t time){
    char buf[128] = {0};
    int64_t seconds = time/1000000;
    tm t;
    localtime_r(&seconds,&t);
    snprintf(buf,128,"%4d/%02d/%02d %02d:%02d:%02d",
        t.tm_year + 1900,
        t.tm_mon + 1,
        t.tm_mday,
        t.tm_hour,
        t.tm_min,
        t.tm_sec
    );
    return buf;
}


inline int64_t string_to_time(const std::string &t)
{
    std::tm tm_result = {0};
    std::istringstream ss(t);
    
    // 解析格式 "2023/10/27 10:30:00"
    ss >> std::get_time(&tm_result, "%Y/%m/%d %H:%M:%S");
    
    if (ss.fail()) {
        return 0; // 解析失败
    }

    time_t seconds = std::mktime(&tm_result);
    if (seconds == -1) return 0;

    return static_cast<int64_t>(seconds);
}