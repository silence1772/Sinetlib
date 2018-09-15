#include "asynclogger.h"
#include "logfile.h"
#include <functional>
#include <stdio.h>
#include <unistd.h>

AsyncLogger::AsyncLogger(const std::string file_name, int flush_interval)
    : flush_interval_(flush_interval),
      is_running_(false),
      file_name_(file_name_),
      thread_(std::bind(&AsyncLogger::ThreadFunc, this), "Logger"),
      mutex_(),
      condition_(mutex_),
      latch_(1),
      cur_buffer_(new FixedBuffer),
      next_buffer_(new FixedBuffer),
      buffers_()
{
    cur_buffer_->Bzero();
    next_buffer_->Bzero();
    buffers_.reserve(16);
}

void AsyncLogger::Append(const char* str, int len)
{
    MutexLockGuard lock(mutex_);
    if (cur_buffer_->GetAvailable() > len)
    {
        cur_buffer_->Append(str, len);
        printf("te\n");
    }
    else
    {
        buffers_.push_back(cur_buffer_);
        cur_buffer_.reset();
        if (next_buffer_)
        {
            cur_buffer_ = std::move(next_buffer_);
        }
        else
        {
            cur_buffer_.reset(new FixedBuffer);
        }
        cur_buffer_->Append(str, len);
        condition_.Notify();
    }
}

void AsyncLogger::ThreadFunc()
{
    latch_.CountDown();
    LogFile output(file_name_);

    BufferPtr back_buffer_1(new FixedBuffer);
    BufferPtr back_buffer_2(new FixedBuffer);
    back_buffer_1->Bzero();
    back_buffer_2->Bzero();

    BufferVector buffers_to_write;
    buffers_to_write.reserve(16);
    while (is_running_)
    {
        {
            MutexLockGuard lock(mutex_);
            if (buffers_.empty())
            {
                condition_.WaitForSeconds(flush_interval_);
            }
            buffers_.push_back(cur_buffer_);
            cur_buffer_.reset();

            cur_buffer_ = std::move(back_buffer_1);
            buffers_to_write.swap(buffers_);
            if (!next_buffer_)
            {
                next_buffer_ = std::move(back_buffer_2);
            }
        }

        if (buffers_to_write.size() > 25)
        {
            buffers_to_write.erase(buffers_to_write.begin() + 2, buffers_to_write.end());
        }

        for (size_t i = 0; i < buffers_to_write.size(); ++i)
        {
            output.Append(buffers_to_write[i]->GetData(), buffers_to_write[i]->GetLength());
        }

        if (buffers_to_write.size() > 2)
        {
            buffers_to_write.resize(2);
        }

        if (!back_buffer_1)
        {
            back_buffer_1 = buffers_to_write.back();
            buffers_to_write.pop_back();
            back_buffer_1->Reset();
        }

        if (!back_buffer_2)
        {
            back_buffer_2 = buffers_to_write.back();
            buffers_to_write.pop_back();
            back_buffer_2->Reset();
        }

        buffers_to_write.clear();
        output.Flush();
    }
    output.Flush();
}