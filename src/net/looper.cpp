#include "looper.h"
#include "eventbase.h"
#include "epoller.h"


Looper::Looper() :
    quit_(false),
    epoller_(new Epoller())
{}

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
    }
}

// 注册事件
void Looper::AddEventBase(std::shared_ptr<EventBase> eventbase)
{
    eventbase_list_.push_back(eventbase);
    epoller_->Add(eventbase);
}

void Looper::RunTask(Task&& task)
{
    task();
}

void Looper::AddTask(Task&& task)
{
    {
        std::unique_lock<std::mutex> lock(mutex_);
        task_queue_.push_back(task);
    }
}