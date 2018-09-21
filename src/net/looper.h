#ifndef LOOPER_h
#define LOOPER_h

#include <vector>
#include <memory>
#include <functional>
#include <mutex>

class EventBase;
class Epoller;

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

    void RunTask(Task&& task);
    void AddTask(Task&& task);

    
private:
    bool quit_;
    std::unique_ptr<Epoller> epoller_;

    // 注册的事件列表
    std::vector<std::shared_ptr<EventBase>> eventbase_list_;

    std::mutex mutex_;
    std::vector<Task> task_queue_;
};

#endif // LOOPER_h