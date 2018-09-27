#ifndef LOOPER_h
#define LOOPER_h

#include "currentthread.h"
#include <vector>
#include <memory>
#include <functional>
#include <mutex>
#include "timer.h"

class EventBase;
class Epoller;
class TimerQueue;

class Looper
{
public:
    using Task = std::function<void()>;

    Looper();
    ~Looper();
    
    // 开始事件循环
    void Start();
    // 退出循环
    void Quit() { quit_ = true; }

    // 注册事件
    void AddEventBase(std::shared_ptr<EventBase> eventbase);
    void ModEventBase(std::shared_ptr<EventBase> eventbase);
    void DelEventBase(std::shared_ptr<EventBase> eventbase);

    void WakeUp();
    void HandleWakeUp();

    void RunTask(Task&& task);
    void AddTask(Task&& task);

    void RunTaskAfter(Task&& task, Nanosecond interval);

    void HandleTask();

    bool IsInBaseThread() const { return thread_id_ == CurrentThread::GetTid(); }
private:
    bool quit_;
    bool is_handle_task_;
    int wakeup_fd_;
    std::shared_ptr<EventBase> wakeup_eventbase_;
    const pid_t thread_id_;
    std::unique_ptr<Epoller> epoller_;
    std::unique_ptr<TimerQueue> timer_queue_;

    // 注册的事件列表
    //std::vector<std::shared_ptr<EventBase>> eventbase_list_;

    std::mutex mutex_;
    std::vector<Task> task_queue_;
};

#endif // LOOPER_h