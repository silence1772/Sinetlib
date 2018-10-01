#include "timerqueue.h"
#include "eventbase.h"
#include "looper.h"
#include "logger.h"
#include "timer.h"
#include "util.h"
#include <sys/timerfd.h>
#include <string.h> // for bzero()
#include <unistd.h> // for read()
#include <ratio> // std::nano::den

TimerQueue::TimerQueue(Looper* loop) :
    loop_(loop),
    timerfd_(timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC)),
    timer_eventbase_(std::make_shared<EventBase>(timerfd_))
{
    timer_eventbase_->SetReadCallback(std::bind(&TimerQueue::HandelTimerExpired, this));
    timer_eventbase_->EnableReadEvents();
    loop_->AddEventBase(timer_eventbase_);
}

TimerQueue::~TimerQueue()
{
    util::Close(timerfd_);
}

void TimerQueue::AddTimer(const std::function<void()>& cb, Timestamp when)
{
    auto timer = std::make_shared<Timer>(cb, when);
    
    // 新的定时器到期时间小于队列中最小的那个的话，则更新timerfd到期时间
    if (timer_queue_.empty() || timer->GetExpTime() < timer_queue_.top()->GetExpTime())
    {
        ResetTimerfd(timerfd_, timer->GetExpTime());
    }

    timer_queue_.push(timer);
}

void TimerQueue::ResetTimerfd(int timerfd, Timestamp when)
{
    // 计算多久后到期
    struct timespec ts;
    Nanosecond gap = when - system_clock::now();
    ts.tv_sec = static_cast<time_t>(gap.count() / std::nano::den);
    ts.tv_nsec = gap.count() % std::nano::den;

    // 更新timerfd剩余到期时间
    struct itimerspec new_value, old_value;
    bzero(&new_value, sizeof(new_value));
    bzero(&old_value, sizeof(old_value));

    new_value.it_value = ts;
    timerfd_settime(timerfd, 0, &new_value, &old_value);
}

// 处理定时器超时
void TimerQueue::HandelTimerExpired()
{
    // read the timerfd
    uint64_t exp_cnt;
    ssize_t n = read(timerfd_, &exp_cnt, sizeof(exp_cnt));
    if (n != sizeof(exp_cnt))
    {
        LOG_ERROR << "read error";
    }


    Timestamp now(system_clock::now());
    // 处理所有到期的定时器
    while (!timer_queue_.empty() && timer_queue_.top()->GetExpTime() < now)
    {
        timer_queue_.top()->Run();
        timer_queue_.pop();
    }

    // 重设timerfd到期时间
    if (!timer_queue_.empty())
    {
        ResetTimerfd(timerfd_, timer_queue_.top()->GetExpTime());
    }
}