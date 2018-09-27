#include "timerqueue.h"
#include "eventbase.h"
#include "looper.h"
#include "timer.h"
#include <sys/timerfd.h>
#include <unistd.h> // for close()
#include <string.h> // for bzero()
#include <sys/time.h> // gor gettimeofday()
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
    close(timerfd_);
}

void TimerQueue::AddTimer(const std::function<void()>& cb, Timestamp when)
{
    auto timer = std::make_shared<Timer>(cb, when);
    
    if (timer_queue_.empty() || timer->GetExpTime() < timer_queue_.top()->GetExpTime())
    {
        ResetTimerfd(timerfd_, timer->GetExpTime());
    }

    timer_queue_.push(timer);
}

void TimerQueue::ResetTimerfd(int timerfd, Timestamp when)
{
    

    // get gap between now and future
    // struct timeval tv;
    // gettimeofday(&tv, NULL);
    // int64_t microseconds = timestamp - (tv.tv_sec * 1000 * 1000 + tv.tv_usec);
    // if (microseconds < 100)
    //     microseconds = 100;

    struct timespec ts;
    Nanosecond gap = when - system_clock::now();
    ts.tv_sec = static_cast<time_t>(gap.count() / std::nano::den);
    ts.tv_nsec = gap.count() % std::nano::den;

    // update timerfd's remaining time
    struct itimerspec new_value, old_value;
    bzero(&new_value, sizeof(new_value));
    bzero(&old_value, sizeof(old_value));

    new_value.it_value = ts;
    timerfd_settime(timerfd, 0, &new_value, &old_value);
}

void TimerQueue::HandelTimerExpired()
{
    // read the timerfd
    uint64_t exp_cnt;
    ssize_t n = read(timerfd_, &exp_cnt, sizeof(exp_cnt));
    if (n != sizeof(exp_cnt))
    {
        // LOG_SYSERR << "read error";
    }

    // struct timeval tv;
    // gettimeofday(&tv, NULL);
    // int64_t now_microseconds = (tv.tv_sec * 1000 * 1000 + tv.tv_usec);

    Timestamp now(system_clock::now());

    while (!timer_queue_.empty() && timer_queue_.top()->GetExpTime() < now)
    {
        timer_queue_.top()->Run();
        timer_queue_.pop();
    }

    if (!timer_queue_.empty())
    {
        ResetTimerfd(timerfd_, timer_queue_.top()->GetExpTime());
    }
}