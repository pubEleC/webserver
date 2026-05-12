#include "timeropt.h"
#include <memory>
#include <sys/timerfd.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace myutils{

class TimerQueue{
public:
    //只能用指针，因为这个是抽象类，不能用具体的实现
    TimerQueue(std::unique_ptr<ITimer>& timer);

    ~TimerQueue();

    void start();

    TimerId addTimer(Timestamp when, TimerCallback cb);
    
    TimerId addTimerRepeat(Timestamp when, Duration interval, TimerCallback cb);

    void delTimer(TimerId id);
    
    void resetTimerfd(Timestamp when);
    
    //void resetTimerfd();
    
    void exit();
    
private:

    //void startThread();

    std::thread thread_;

    int epollfd_ = -1;
    int timerfd_ = -1;

    std::unique_ptr<ITimer> timer_;

    std::atomic<bool> stop_{0};
    int start_ = 0;

    Timestamp startTs_ = std::chrono::steady_clock::now();

    Timestamp nextTs_ = Timestamp::max();

    std::mutex mutex_;
    std::condition_variable cond_;

};

}