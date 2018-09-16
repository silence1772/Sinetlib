#ifndef ASYNCLOGGER_H
#define ASYNCLOGGER_H

#include "logstream.h"  // Buffer
#include <vector>
#include <string>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <thread>

// 异步日志，单独开启一个线程负责输出日志
class AsyncLogger
{
public:
    // 参数为日志文件名，默认每隔3秒冲刷缓冲区到磁盘文件中
    AsyncLogger(const std::string file_name, int flush_interval = 3);
    ~AsyncLogger()
    {
        if (is_running_)
        {
            Stop();
        }
    }

    // 添加数据到缓冲区
    void Append(const char* str, int len);

    // 开启日志线程
    void Start()
    {
        is_running_ = true;
        thread_ = std::thread(std::bind(&AsyncLogger::ThreadFunc, this));
    }

    // 停止日志线程
    void Stop()
    {
        is_running_ = false;
        // 停止时notify以便将残留在缓存区中的数据输出
        condition_.notify_one();
        thread_.join();
    }
private:
    // 后端日志线程函数
    void ThreadFunc();

    // 固定大小缓冲区
    typedef Buffer<LARGE_BUFFER_SIZE> FixedBuffer;
    typedef std::vector<std::shared_ptr<FixedBuffer>> BufferVector;
    typedef std::shared_ptr<FixedBuffer> BufferPtr;

    // 冲刷间隔
    const int flush_interval_;
    // log线程是否正在运行
    bool is_running_;
    // 日志文件名
    std::string file_name_;

    // log线程
    std::thread thread_;
    // 互斥锁
    std::mutex mutex_;
    // 条件变量，等待的是缓冲区内容可以给log线程享用的条件发生
    std::condition_variable condition_;

    // 当前缓冲区指针
    BufferPtr cur_buffer_;
    // 下一个可用的缓冲区指针
    BufferPtr next_buffer_;
    // 准备交付给后端log线程使用的缓冲区
    BufferVector buffers_;
};

#endif // ASYNCLOGGER_H