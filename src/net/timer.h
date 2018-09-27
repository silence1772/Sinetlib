#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include <functional>
#include <chrono>

using std::chrono::system_clock;
using Nanosecond = std::chrono::nanoseconds;
using Second = std::chrono::seconds;
using Timestamp = std::chrono::time_point<system_clock, Nanosecond>;

class Timer
{
public:
    Timer(const std::function<void()>& cb, Timestamp when) :
        callback_(cb),
        when_(when) {}
        // expiration_timestamp_(timestamp),
        // interval_(interval),
        // is_repeat_(interval > 0.0) {}
    
    void Run() const { callback_(); }
    Timestamp GetExpTime() const { return when_; }
    // Timestamp GetExpirationTimestamp() const {return expiration_timestamp_; }
    // bool IsRepeat() const { return is_repeat_; }
    // void Continue();


private:
    const std::function<void()> callback_;
    Timestamp when_;
    // const double interval_;
    // const bool is_repeat_;
};

#endif // TIMER_H