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
    
    void Start();
    void Quit() { quit_ = true; }
    void RunTask(Task&& task);
    void AddTask(Task&& task);

    void AddEventBase(std::shared_ptr<EventBase> eventbase);
private:
    bool quit_;
    std::unique_ptr<Epoller> epoller_;
    std::mutex mutex_;
    std::vector<Task> task_queue_;
    std::vector<std::shared_ptr<EventBase>> eventbase_list_;
    
};

#endif // LOOPER_h