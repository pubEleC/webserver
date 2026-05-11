#pragma once
#include "timeropt.h"
#include <queue>
#include <unordered_map>
#include <vector>
#include <atomic>

class PriorityTimer : public ITimer{
public:

    PriorityTimer();

    ~PriorityTimer();

    //void start();

    // 添加一个定时任务
    TimerId addTimer(Timestamp when, TimerCallback cb) override;
    // 添加一个周期性定时任务 (便捷接口)
    TimerId addTimerRepeat(Timestamp when, Duration interval, TimerCallback cb) override;

    void cancel(TimerId id) override;
    // 获取当前最早的定时任务还有多久触发
    Duration timeToNextExpired() const override;

    Timestamp firstTimer() const override;
    // 处理定时任务
    void tick() override;

private:

    struct TimerNode {
        TimerId id;             // 唯一标识
        Timestamp expire;       // 到期时间点
        Duration interval;      // 周期间隔（如果是周期任务，值>0；单次任务，值==0）
        TimerCallback cb;       // 回调函数
    };

    // 自定义比较器：让时间最小的排在堆顶 (优先队列默认是最大堆，我们要反着来)
    struct TimerCompare {
        bool operator()(const TimerNode& lhs, const TimerNode& rhs) const {
            return lhs.expire > rhs.expire; 
        }
    };

    // 优先队列（底层是 vector + 堆算法）
    std::priority_queue<TimerNode, std::vector<TimerNode>, TimerCompare> heap_;

    // 懒删除的核心：记录被取消的定时器ID
    std::unordered_map<TimerId, bool> canceled_;
};