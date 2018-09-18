#ifndef CONDITION_H
#define CONDITION_H

#include "mutexlock.h"
#include <pthread.h>
#include <time.h>
#include <errno.h>

// 条件变量包装类
class Condition
{
public:
    explicit Condition(MutexLock& mutex) : mutex_(mutex)
    {
        // 初始化条件变量
        pthread_cond_init(&pcon_, NULL);
    }
    
    ~Condition()
    {
        // 销毁条件变量，释放资源
        pthread_cond_destroy(&pcon_);
    }
    
    void Wait()
    {
        // 等待条件变量
        pthread_cond_wait(&pcon_, mutex_.Get());
    }

    bool WaitForSeconds(int seconds)
    {
        struct timespec abstime;
        clock_gettime(CLOCK_REALTIME, &abstime);
        abstime.tv_sec += static_cast<time_t>(seconds);
        return ETIMEDOUT == pthread_cond_timedwait(&pcon_, mutex_.Get(), &abstime);
    }
    
    void Notify()
    {
        // 唤醒等待条件变量的线程
        pthread_cond_signal(&pcon_);
    }
    
    void NotifyAll()
    {
        // 唤醒所有等待条件变量的线程
        pthread_cond_broadcast(&pcon_);
    }
    
private:
    // 保护条件变量的互斥锁
    MutexLock& mutex_;
    // 条件变量
    pthread_cond_t pcon_;
};

#endif // CONDITION_H