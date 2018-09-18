#include "asynclogger.h"
#include "logfile.h"
#include <stdio.h>
#include <unistd.h>
#include <functional>
#include <chrono>

// 初始化成员变量
AsyncLogger::AsyncLogger(const std::string file_name, int flush_interval)
    : flush_interval_(flush_interval),
      is_running_(false),
      file_name_(file_name),
      thread_(),
      mutex_(),
      condition_(),
      cur_buffer_(new FixedBuffer),
      next_buffer_(new FixedBuffer),
      buffers_()
{
    cur_buffer_->Bzero();
    next_buffer_->Bzero();
    buffers_.reserve(16);
}

// 添加数据到缓冲区
void AsyncLogger::Append(const char* str, int len)
{
    std::unique_lock<std::mutex> lock(mutex_);
    // 当前缓冲区有足够空间，直接添加
    if (cur_buffer_->GetAvailable() > len)
    {
        cur_buffer_->Append(str, len);
    }
    else
    {
        // 把已满的缓冲区放入预备给后端log线程的容器中
        buffers_.push_back(cur_buffer_);
        // 将cur_buffer_的指针置为空
        cur_buffer_.reset();
        // 如何下一个可用缓冲区不为空，则交给cur
        if (next_buffer_)
        {
            cur_buffer_ = std::move(next_buffer_);
        }
        // 否则新建一个
        else
        {
            cur_buffer_.reset(new FixedBuffer);
        }
        cur_buffer_->Append(str, len);
        // 提醒后端log线程可以读取缓冲区了
        condition_.notify_one();
    }
}

// 后端log线程
void AsyncLogger::ThreadFunc()
{
    // 打开日志文件
    LogFile output(file_name_);

    // 后端备用缓冲区1、2
    BufferPtr back_buffer_1(new FixedBuffer);
    BufferPtr back_buffer_2(new FixedBuffer);
    back_buffer_1->Bzero();
    back_buffer_2->Bzero();

    // 预备读取的缓冲区
    BufferVector buffers_to_write;
    buffers_to_write.reserve(16);

    while (is_running_)
    {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            if (buffers_.empty())
            {
                // 等待flush_interval_秒
                condition_.wait_for(lock, std::chrono::seconds(flush_interval_));
            }

            // 直接把cur_buffer_放入容器中
            buffers_.push_back(cur_buffer_);
            cur_buffer_.reset();
            // 将后端log线程的缓冲区1交给cur
            cur_buffer_ = std::move(back_buffer_1);

            // 把buffers跟后端log线程空的容器交换
            buffers_to_write.swap(buffers_);

            // 如果下一个可用缓冲区为空，就把后端log线程的缓冲区2交给next
            if (!next_buffer_)
            {
                next_buffer_ = std::move(back_buffer_2);
            }
        }

        // 如果缓冲区过多，说明前端产生log的速度远大于后端消费的速度，这里只是简单的将它们丢弃
        if (buffers_to_write.size() > 25)
        {
            buffers_to_write.erase(buffers_to_write.begin() + 2, buffers_to_write.end());
        }

        // 将缓冲区内容写入文件
        for (size_t i = 0; i < buffers_to_write.size(); ++i)
        {
            output.Append(buffers_to_write[i]->GetData(), buffers_to_write[i]->GetLength());
        }

        // 将过多的缓冲区丢弃
        if (buffers_to_write.size() > 2)
        {
            buffers_to_write.resize(2);
        }

        // 恢复后端备用缓冲区
        if (!back_buffer_1)
        {
            back_buffer_1 = buffers_to_write.back();
            buffers_to_write.pop_back();
            // 将缓冲区的数据指针归零
            back_buffer_1->Reset();
        }

        if (!back_buffer_2)
        {
            back_buffer_2 = buffers_to_write.back();
            buffers_to_write.pop_back();
            back_buffer_2->Reset();
        }

        // 丢弃无用的缓冲区
        buffers_to_write.clear();
        output.Flush();
    }
    output.Flush();
}