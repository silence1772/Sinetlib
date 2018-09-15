#ifndef COUNTDOWNLATCH_H
#define COUNTDOWNLATCH_H

#include "mutexlock.h"
#include "condition.h"


class CountDownLatch
{
public:
    explicit CountDownLatch(int count);
    void Wait();
    void CountDown();

private:
    mutable MutexLock mutex_;
    Condition condition_;
    int count_;
};

#endif // COUNTDOWNLATCH_H