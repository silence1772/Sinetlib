#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include <functional>

class Timer
{
public:
    Timer(const std::function<void()>& cb, int64_t micro_second_timestamp) :
        callback_(cb),
        micro_second_timestamp_(micro_second_timestamp) {}
        // expiration_timestamp_(timestamp),
        // interval_(interval),
        // is_repeat_(interval > 0.0) {}
    
    void Run() const { callback_(); }
    int64_t GetExpirationTimestamp() const { return micro_second_timestamp_; }
    // Timestamp GetExpirationTimestamp() const {return expiration_timestamp_; }
    // bool IsRepeat() const { return is_repeat_; }
    // void Continue();
    friend bool operator>(const Timer& lhs, const Timer& rhs) const
    {
        return lhs.micro_second_timestamp_ > rhs.micro_second_timestamp_;
    }

private:
    const std::function<void()> callback_;
    int64_t micro_second_timestamp_;
    // const double interval_;
    // const bool is_repeat_;
};

#endif // TIMER_H