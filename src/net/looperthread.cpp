#include "looperthread.h"
#include "looper.h"

LooperThread::LooperThread() :
    loop_(nullptr),
    // 初始化后线程便开始运行
    thread_(std::bind(&LooperThread::ThreadFunc, this)),
    mutex_(),
    condition_()
{}

LooperThread::~LooperThread()
{
    loop_->Quit();
    thread_.join();
}

Looper* LooperThread::GetLoop()
{
    // 互斥锁与条件变量配合使用，当新线程运行起来后才能够得到loop的指针
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while (loop_ == nullptr)
        {
            condition_.wait(lock);
        }
    }
    return loop_;
}

void LooperThread::ThreadFunc()
{
    Looper loop;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        loop_ = &loop;
        // 得到了loop的指针，通知wait
        condition_.notify_one();
    }
    loop.Start();
}