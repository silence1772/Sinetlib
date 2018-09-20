#ifndef EPOLLER_H
#define EPOLLER_H

#include <vector>
#include <map>
#include <memory>
#include <sys/epoll.h>

class EventBase;

class Epoller
{
public:

    Epoller();
    ~Epoller();
    
    void Add(std::shared_ptr<EventBase> eventbase);
    void Mod(std::shared_ptr<EventBase> eventbase);
    void Del(std::shared_ptr<EventBase> eventbase);

    std::vector<std::shared_ptr<EventBase>> Poll();

private:
    int epollfd_;

    std::vector<epoll_event> active_event_;
    std::map<int, std::shared_ptr<EventBase>> fd_2_eventbase_list_;
};

#endif // EPOLL_H