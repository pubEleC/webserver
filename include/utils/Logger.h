#pragma once

#include <string>

#include "noncopyable.h"
#include <iostream>
#include "Timestamp.h"
inline std::string get_cur_file_name(const std::string& name){
    size_t length = name.size()-1;
    size_t R = length;
    size_t count = 0;
    while(R){
        if(name[R] == '/'){
            count++;
            if(count == 3){
                return name.substr(R,length);
            }
        }
        R--;
    }
    return name;
}



#define CUR_FILE_NAME get_cur_file_name(__FILE__).c_str()

#define LOG_LOG(level,_format,...) \
do\
{\
    char buf[1024] = {0}; \
    int count = snprintf(buf, 1024, "[%s][%s][%s][%s][%d]",level,TimeStamp::now().toString().c_str(), CUR_FILE_NAME,__FUNCTION__,__LINE__); \
    snprintf(buf+count, 1024-count, _format, ##__VA_ARGS__); \
    std::cout << buf << std::endl;\
}while(0)

//"[%s][%s][%d]%s",CUR_FILE_NAME,__FUNCTION__,__LINE__,
//#define LOG(_format,...) LOG_INFO2(_format,##__VA_ARGS__);
    
#define LOG_DEBUG(_format, ...) LOG_LOG("DEBUG", _format, ##__VA_ARGS__)
#define LOG_INFO(_format, ...)  LOG_LOG("INFO", _format, ##__VA_ARGS__)
#define LOG_ERROR(_format, ...) LOG_LOG("ERROR", _format, ##__VA_ARGS__)
#define LOG_FATAL(_format, ...) LOG_LOG("FATAL", _format, ##__VA_ARGS__)

#define LOG(str) \
do\
{\
    char buf[1024] = {0}; \
    int count = snprintf(buf, 1024, "[INFO][%s][%s][%s][%d]", TimeStamp::now().toString().c_str(),CUR_FILE_NAME,__FUNCTION__,__LINE__); \
    std::cout << buf << str << std::endl;\
}while(0)



//定义日志级别  INFO ERROR FATAL DEBUG(较多)
enum LogLevel
{
    INFO,
    ERROR,
    FATAL,
    DEBUG
};

class Logger : noncopyable{
public:
    static Logger& instance();

    void setLogLevel(int level);

    void log(const std::string& msg);

private:

    int logLevel_;

    Logger(){

    }
};