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
    void ThreadFunc();

    Looper* loop_;
    std::thread thread_;

    std::mutex mutex_;
    std::condition_variable condition_;
};

#endif // LOOPER_THREAD_H