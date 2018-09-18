#include "countdownlatch.h"

CountDownLatch::CountDownLatch(int count)
  : mutex_(),
    condition_(),
    count_(count) {}

void CountDownLatch::Wait()
{
    // RAII手法实现析构时自动解锁，unique_lock是lock_guard的灵活版
    std::unique_lock<std::mutex> lock(mutex_);
    while (count_ > 0)
    {
        // wait会将调用线程放入condition_的等待队列，然后解锁mutex_
        condition_.wait(lock);
        // 当在他处调用notify时wait函数返回，并且会重新对mutex_上锁
    }
    // 当退出此函数后lock会被析构，同时对mutex_解锁
}

void CountDownLatch::CountDown()
{
    std::unique_lock<std::mutex> lock(mutex_);
    --count_;
    if (count_ == 0)
    {   
        condition_.notify_all();
    }
}