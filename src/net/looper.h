#ifndef LOOPER_H
#define LOOPER_H

#include "timestamp.h"
#include "epoller.h"
#include <vector>
#include <memory>
#include <functional>
#include <mutex>
#include <thread>

class EventBase;
class TimerQueue;

class Looper
{
public:
    using Task = std::function<void()>;

    Looper();
    ~Looper();
    
    // 开始事件循环
    void Start();
    // 退出循环，由其他线程调用
    void Quit() { quit_ = true; }

    // 注册事件
    void AddEventBase(std::shared_ptr<EventBase> eventbase) { epoller_->Add(eventbase); }
    void ModEventBase(std::shared_ptr<EventBase> eventbase) { epoller_->Mod(eventbase); }
    void DelEventBase(std::shared_ptr<EventBase> eventbase) { epoller_->Del(eventbase); }

    // 唤醒循环以处理Task,配合Handle使用
    void WakeUp();
    void HandleWakeUp();

    void RunTask(Task&& task);
    void AddTask(Task&& task);
    void HandleTask();

    void RunTaskAfter(Task&& task, Nanosecond interval);

    bool IsInBaseThread() const { return thread_id_ == std::this_thread::get_id(); }
private:
    // 退出标识
    bool quit_;
    // 是否正在出来任务
    bool is_handle_task_;

    // 用来唤醒的描述符以及关注该描述符的事件基类
    int wakeup_fd_;
    std::shared_ptr<EventBase> wakeup_eventbase_;

    // 该loop所在线程id
    const std::thread::id thread_id_;

    std::unique_ptr<Epoller> epoller_;
    // 定时器队列
    std::unique_ptr<TimerQueue> timer_queue_;

    // 任务队列以及保护该队列的互斥锁
    std::mutex mutex_;
    std::vector<Task> task_queue_;
};

#endif // LOOPER_H