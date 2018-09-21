#include "threadpool.h"
#include "looper.h"
#include "looperthread.h"


ThreadPool::ThreadPool(Looper* base_loop, int thread_num) :
    base_loop_(base_loop),
    thread_num_(thread_num),
    next_(0)
{
    //assert(thread_num_ > 0);
}

ThreadPool::~ThreadPool() {}

void ThreadPool::Start()
{
    for (int i = 0; i < thread_num_; ++i)
    {
        std::unique_ptr<LooperThread> t(new LooperThread());
        loopers_.push_back(t->GetLoop());
        looper_threads_.push_back(std::move(t));
    }
}

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