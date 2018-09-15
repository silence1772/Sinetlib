#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>
#include <functional>
#include <string>
#include "atomic.h"
#include "countdownlatch.h"

class Thread
{
public:
    // 线程调用函数类型
    typedef std::function<void ()> ThreadFunc;
    explicit Thread(ThreadFunc func, const std::string& name = std::string());
    ~Thread();

    // 开启线程
    void Start();
    // 等待结束线程
    int Join();
    // 是否开启
    bool IsStarted() const { return is_started_; }
    // 获取线程id
    pid_t GetTid() const { return tid_; }
    // 获取线程名称
    const std::string& GetName() const { return name_; }
    // 获取递增计数值
    static int GetNumCreated() { return num_created_.Get(); }

private:
    static void* StartThread(void* obj);
    void RunInThread();

    // 是否开启线程
    bool is_started_;
    // 是否结束线程
    bool is_joined_;
    // 线程id
    pthread_t pthread_id_;
    // 进程id
    pid_t tid_;
    // 线程函数
    ThreadFunc thread_func_;
    // 线程名称
    std::string name_;
    // 递增计数值，用于线程名称
    static Atomic<int64_t> num_created_;
    CountDownLatch latch_;
};

#endif // THREAD_H