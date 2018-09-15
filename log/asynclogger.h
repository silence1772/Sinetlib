#ifndef ASYNCLOGGER_H
#define ASYNCLOGGER_H

#include <vector>
#include <string>
#include <memory>
#include "mutexlock.h"
#include "thread.h"
#include "logstream.h"
#include "countdownlatch.h"

class AsyncLogger
{
public:
    AsyncLogger(const std::string file_name, int flush_interval = 2);
    ~AsyncLogger()
    {
        if (is_running_)
        {
            Stop();
        }
    }

    void Append(const char* str, int len);

    void Start()
    {
        printf("ss");
        is_running_ = true;
        thread_.Start();
        latch_.Wait();
    }

    void Stop()
    {
        is_running_ = false;
        condition_.Notify();
        thread_.Join();
    }
private:
    void ThreadFunc();

    
    typedef Buffer<LARGE_BUFFER_SIZE> FixedBuffer;
    typedef std::vector<std::shared_ptr<FixedBuffer>> BufferVector;
    typedef std::shared_ptr<FixedBuffer> BufferPtr;

    const int flush_interval_;
    bool is_running_;
    std::string file_name_;

    Thread thread_;
    MutexLock mutex_;
    Condition condition_;
    CountDownLatch latch_;

    BufferPtr cur_buffer_;
    BufferPtr next_buffer_;
    BufferVector buffers_;
};

#endif // ASYNCLOGGER_H