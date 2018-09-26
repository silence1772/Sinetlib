#ifndef TIMERQUEUE_H
#define TIMERQUEUE_H

#include <stdint.h>
#include <set>
#include <vector>
#include <functional>
#include "eventbase.h"

class Looper;
class Timer;

struct TimerCmp
{
    bool operator()(std::shared_ptr<Timer>& lhs, std::shared_ptr<Timer>& rhs)
    {
        return lhs->GetExpirationTimestamp() > rhs->GetExpirationTimestamp();
    }
};

class TimerQueue
{
public:
    TimerQueue(Looper* loop);
    ~TimerQueue();
    
    void AddTimer(const std::function<void()>& cb, int64_t timestamp);

private:
    void ResetTimerfd(int timerfd, int64_t timestamp);
    
    void HandelTimerExpired();
    std::vector<Entry> GetExpired(Timestamp now_timestamp);

    

    Looper* loop_;
    const int timerfd_;
    std::shared_ptr<EventBase> timer_eventbase_;
    

    using TimerPtr = std::shared_ptr<Timer>;
    std::priority_queue<TimerPtr, std::vector<TimerPtr>, greater<TimerCmp>> timer_queue_;
};

#endif // TIMERQUEUE_H