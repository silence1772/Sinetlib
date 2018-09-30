#include "looper.h"
#include "eventbase.h"
#include "timerqueue.h"
#include "logger.h"
#include <sys/eventfd.h>
#include <unistd.h>

Looper::Looper() :
    quit_(false),
    is_handle_task_(false),
    wakeup_fd_(eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC)),
    wakeup_eventbase_(std::make_shared<EventBase>(wakeup_fd_)),
    thread_id_(std::this_thread::get_id()),
    epoller_(new Epoller()),
    timer_queue_(new TimerQueue(this))
{
    if (wakeup_fd_ < 0)
    {
        LOG_FATAL << "create wakeup_fd_ failed";
    }
    // 关注可读事件
    wakeup_eventbase_->SetReadCallback(std::bind(&Looper::HandleWakeUp, this));
    wakeup_eventbase_->EnableReadEvents();
    // 注册到epoller上
    AddEventBase(wakeup_eventbase_);
}

Looper::~Looper() {}

// 开始循环
void Looper::Start()
{
    std::vector<std::shared_ptr<EventBase>> active_eventbase_list;

    while (!quit_)
    {
        active_eventbase_list.clear();
        active_eventbase_list = epoller_->Poll();
        for (auto& it : active_eventbase_list)
        {
            it->HandleEvent();
        }
        HandleTask();
    }
}

// 唤醒循环，由其他线程调用
void Looper::WakeUp()
{
    uint64_t one = 1;
    ssize_t n = write(wakeup_fd_, &one, sizeof(one));
    if (n != sizeof(one))
    {
        LOG_ERROR << "wake up error in write()";
    }
}

// 处理唤醒事件
void Looper::HandleWakeUp()
{
    uint64_t one = 1;
    ssize_t n = read(wakeup_fd_, &one, sizeof(one));
    if (n != sizeof(one))
    {
        LOG_ERROR << "handle wake up error in read()";
    }
}

// 运行Task，可由其他线程调用
void Looper::RunTask(Task&& task)
{
    // 如果不是在自己所在线程调用，则添加进任务队列后唤醒该loop进行处理
    if (IsInBaseThread())
        task();
    else
        AddTask(std::move(task));
}

// 添加任务
void Looper::AddTask(Task&& task)
{
    {
        std::unique_lock<std::mutex> lock(mutex_);
        task_queue_.emplace_back(std::move(task));
    }
    if (!IsInBaseThread() || is_handle_task_)
    {
        WakeUp();
    }
}

// 执行任务队列
void Looper::HandleTask()
{
    is_handle_task_ = true;
    std::vector<Task> tasks;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        tasks.swap(task_queue_);
    }
    for (size_t i = 0; i < tasks.size(); ++i)
    {
        tasks[i]();
    }
    is_handle_task_ = false;
}

// 一定时间后执行该任务，非线程安全，只能在looper所在线程调用
void Looper::RunTaskAfter(Task&& task, Nanosecond interval)
{
    timer_queue_->AddTimer(task, system_clock::now() + interval);
}

