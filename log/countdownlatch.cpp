#include "countdownlatch.h"

CountDownLatch::CountDownLatch(int count)
  : mutex_(),
    condition_(mutex_),
    count_(count) {}

void CountDownLatch::Wait()
{
    MutexLockGuard lock(mutex_);
    while (count_ > 0)
    {
        condition_.Wait();
    }
}

void CountDownLatch::CountDown()
{
    MutexLockGuard lock(mutex_);
    --count_;
    if (count_ == 0)
    {   
        condition_.NotifyAll();
    }
}