#include "thread.h"
#include <pthread.h>
#include "currentthread.h"

Atomic<int64_t> Thread::num_created_;

Thread::Thread(ThreadFunc func, const std::string& name)
    : is_started_(false),
      is_joined_(false),
      pthread_id_(0),
      tid_(0),
      thread_func_(std::move(func)),
      name_(name),
      latch_(1)
{
    // 生成线程默认名称
    int num = num_created_.IncreaseAndGet();
    if (name_.empty())
    {
        char buf[32];
        snprintf(buf, sizeof buf, "Thread%d", num);
        name_ = buf;
    }
}

Thread::~Thread()
{
    // 当线程仍未结束时分离线程，让其结束时自行释放资源
    if (is_started_ && !is_joined_)
    {
        pthread_detach(pthread_id_);
    }
}

// 线程函数入口
void* Thread::StartThread(void* obj)
{
    Thread* thread = static_cast<Thread*>(obj);
    thread->RunInThread();
    return NULL;
}
 
void Thread::RunInThread()
{
    latch_.CountDown();
    CurrentThread::t_thread_name = name_.empty() ? "defaultThread" : name_.c_str();

    thread_func_();

    CurrentThread::t_thread_name = "finished";
}

// 开启线程
void Thread::Start()
{
    is_started_ = true;
    if (pthread_create(&pthread_id_, NULL, StartThread, this))
    {
        is_started_ = false;
    }
    else
    {
        latch_.Wait();
    }
}

// 等待结束线程
int Thread::Join()
{
    is_joined_ = true;
    return pthread_join(pthread_id_, NULL);
}