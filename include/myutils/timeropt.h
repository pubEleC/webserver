/**
 *  程序：定时器任务 
 *  作者：pubEleC
 */
#pragma once
#include <chrono>
#include <functional>
#include <atomic>

namespace myutils{

using Timestamp = std::chrono::steady_clock::time_point;
using Duration = std::chrono::steady_clock::duration;
using TimerId = uint64_t;
using TimerCallback = std::function<void()>;

class ITimer {
public:

virtual ~ITimer() = default;

    // 添加一个定时任务
    virtual TimerId addTimer(Timestamp when, TimerCallback cb) = 0;
    
    // 添加一个周期性定时任务 (便捷接口)
    virtual TimerId addTimerRepeat(Timestamp when, Duration interval, TimerCallback cb) = 0;
    
    virtual void cancel(TimerId id) = 0;
    
    // 获取当前最早的定时任务还有多久触发
    // (这个主要是为了配合 timerfd / epoll 使用，用来设置 timerfd_settime 的超时时间)
    // 如果没有定时任务，返回一个特殊值（如 Duration::max()）
    virtual Duration timeToNextExpired() const = 0;
    
    virtual Timestamp firstTimer() const = 0;

    // 处理定时任务
    // (这个函数通常在 epoll_wait 返回，发现 timerfd 可读时调用)
    virtual void tick() = 0;
    
protected:
    
    static std::atomic<TimerId> timerId_;

    //int stop_;
};

}