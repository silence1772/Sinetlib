#ifndef LOOPER_THREAD_H
#define LOOPER_THREAD_H

#include <thread>
#include <mutex>
#include <condition_variable>

class Looper;

class LooperThread
{
public:
    LooperThread();
    ~LooperThread();
    
    Looper* GetLoop();
private:
    // 新线程所运行的函数，即运行looper
    void ThreadFunc();

    Looper* loop_;
    std::thread thread_;

    // 互斥锁与条件变量配合使用
    std::mutex mutex_;
    std::condition_variable condition_;
};

#endif // LOOPER_THREAD_H