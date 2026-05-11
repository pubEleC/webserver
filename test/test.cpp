#include "utils/timeOpt.h"
#include "utils/Logger.h"



int main(){

    int64_t time = get_now_time(timeType::s);

    LOG_INFO("%ld",time);

    std::string str = time_to_String(time*1000000);

    LOG_INFO("%s",str.c_str());

    int64_t time2 = string_to_time(str);

    LOG_INFO("%ld",time2);

}
