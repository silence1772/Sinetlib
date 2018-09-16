#ifndef COUNTDOWNLATCH_H
#define COUNTDOWNLATCH_H

#include <mutex>
#include <condition_variable>

// 倒数计数类，主要用于主线程等待工作线程完成后再执行
// 这里主要是用在开启新线程时，当新线程真正运行后原线程才继续
class CountDownLatch
{
public:
    explicit CountDownLatch(int count);
    void Wait();
    void CountDown();

private:
	// 互斥锁，与条件变量配合使用
    std::mutex mutex_;
    // 条件变量
    std::condition_variable condition_;
    // 等待完成的工作线程数
    int count_;
};

#endif // COUNTDOWNLATCH_H