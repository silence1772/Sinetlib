#include "looper.h"
#include "eventbase.h"
#include "epoller.h"
#include <sys/eventfd.h>
#include <unistd.h>
#include "timerqueue.h"
#include <sys/time.h> // gor gettimeofday()

Looper::Looper() :
    quit_(false),
    is_handle_task_(false),
    wakeup_fd_(eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC)),
    wakeup_eventbase_(std::make_shared<EventBase>(wakeup_fd_)),
    thread_id_(std::this_thread::get_id()),
    epoller_(new Epoller()),
    timer_queue_(new TimerQueue(this))
{
    wakeup_eventbase_->SetReadCallback(std::bind(&Looper::HandleWakeUp, this));
    wakeup_eventbase_->EnableReadEvents();
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

// 注册事件
void Looper::AddEventBase(std::shared_ptr<EventBase> eventbase)
{
    //eventbase_list_.push_back(eventbase);
    epoller_->Add(eventbase);
}

void Looper::ModEventBase(std::shared_ptr<EventBase> eventbase)
{
    epoller_->Mod(eventbase);
}

void Looper::DelEventBase(std::shared_ptr<EventBase> eventbase)
{
    epoller_->Del(eventbase);
}

void Looper::WakeUp()
{
    uint64_t one = 1;
    ssize_t n = write(wakeup_fd_, &one, sizeof(one));
    if (n != sizeof(one))
    {
        //LOG_SYSERR << "write error";
    }
}
void Looper::HandleWakeUp()
{
    uint64_t one = 1;
    ssize_t n = read(wakeup_fd_, &one, sizeof(one));
    if (n != sizeof(one))
    {
        //LOG_SYSERR << "read error";
    }
}

void Looper::RunTask(Task&& task)
{
    if (IsInBaseThread())
        task();
    else
        AddTask(std::move(task));
}

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

void Looper::RunTaskAfter(Task&& task, Nanosecond interval)
{
    timer_queue_->AddTimer(task, system_clock::now() + interval);
}

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