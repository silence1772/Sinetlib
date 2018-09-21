#ifndef EPOLLER_H
#define EPOLLER_H

#include <sys/epoll.h>
#include <vector>
#include <map>
#include <memory>

// 前向声明
class EventBase;

class Epoller
{
public:
    Epoller();
    ~Epoller();
    
    // 往epoll的事件表上注册事件
    void Add(std::shared_ptr<EventBase> eventbase);
    // 修改注册的事件
    void Mod(std::shared_ptr<EventBase> eventbase);
    // 删除注册的事件
    void Del(std::shared_ptr<EventBase> eventbase);

    // 等待事件发生
    std::vector<std::shared_ptr<EventBase>> Poll();

private:
    // epoll自身的文件描述符，用来唯一标识内核中的epoll事件表
    int epollfd_;

    // 传给epoll_wait的参数，epoll_wait将会把发生的事件写入到这个列表中返回给用户
    std::vector<epoll_event> active_event_;
    // 从fd到eventbase的映射列表，这样可以方便的根据active_event_中的fd来得到对应的eventbase
    std::map<int, std::shared_ptr<EventBase>> fd_2_eventbase_list_;
};

#endif // EPOLL_H