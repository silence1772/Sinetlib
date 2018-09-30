#include "threadpool.h"
#include "looper.h"
#include "looperthread.h"


ThreadPool::ThreadPool(Looper* base_loop, int thread_num) :
    base_loop_(base_loop),
    thread_num_(thread_num),
    next_(0) 
{}

ThreadPool::~ThreadPool() {}

// 启动线程池，即创建相应数量的线程放入池中
void ThreadPool::Start()
{
    for (int i = 0; i < thread_num_; ++i)
    {
        std::unique_ptr<LooperThread> t(new LooperThread());
        loopers_.push_back(t->GetLoop());
        looper_threads_.push_back(std::move(t));
    }
}

// 取出loop消费，简单的循环取用
Looper* ThreadPool::TakeOutLoop()
{
    Looper* loop = base_loop_;
    if (!loopers_.empty())
    {
        loop = loopers_[next_];
        next_ = (next_ + 1) % thread_num_;
    }
    return loop;
}