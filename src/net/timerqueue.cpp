#include "timerqueue.h"

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

void TimerQueue::AddTimer(const std::functional<void()>& cb, int64_t timestamp)
{
    auto timer = std::make_shared<Timer>(cb, timestamp);
    
    if (timer_queue_.empty() || *timer < *timer_queue_.top())
    {
        ResetTimerfd(timerfd_, timer->GetExpirationTimestamp());
    }

    timer_queue_.push(timer);
}

void TimerQueue::ResetTimerfd(int timerfd, int64_t timestamp)
{
    struct itimerspec new_value;
    struct itimerspec old_value;
    bzero(&new_value, sizeof(new_value));
    bzero(&old_value, sizeof(old_value));

    // get gap between now and future
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int64_t microseconds = timestamp - (tv.tv_sec * 1000 * 1000 + tv.tv_usec);
    if (microseconds < 100)
        microseconds = 100;

    struct timespec ts;
    ts.tv_sec = static_cast<time_t>(microseconds / (1000 * 1000));
    ts.tv_nsec = static_cast<long>((microseconds % (1000 * 1000)) * 1000);

    // update timerfd's remaining time
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

    struct timeval tv;
    gettimeofday(&tv, NULL);
}