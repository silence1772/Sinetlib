#include "looperthread.h"
#include "looper.h"

LooperThread::LooperThread() :
    loop_(nullptr),
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
        condition_.notify_one();
    }
    loop.Start();
}