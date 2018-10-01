#ifndef TIMERQUEUE_H
#define TIMERQUEUE_H

#include "timer.h"
#include <stdint.h>
#include <vector>
#include <functional>
#include <queue>
#include <memory>

class EventBase;
class Looper;

// 比较器
struct cmp
{
    bool operator()(const std::shared_ptr<Timer>& lhs, const std::shared_ptr<Timer>& rhs)
    {
        return lhs->GetExpTime() > rhs->GetExpTime();
    }
};

class TimerQueue
{
public:
    TimerQueue(Looper* loop);
    ~TimerQueue();
    
    void AddTimer(const std::function<void()>& cb, Timestamp when);

private:
    void ResetTimerfd(int timerfd, Timestamp when);
    
    void HandelTimerExpired();

    Looper* loop_;

    const int timerfd_;
    std::shared_ptr<EventBase> timer_eventbase_;
    
    // 小顶堆实现的定时器队列
    using TimerPtr = std::shared_ptr<Timer>;
    std::priority_queue<TimerPtr, std::vector<TimerPtr>, cmp> timer_queue_;
};

#endif // TIMERQUEUE_H