#include "epoller.h"
#include "eventbase.h"
#include <stdio.h>
#include <iostream>

// epoll_wait最多监听事件数
const int EVENTS_NUM = 4096;
// epoll_wait超时时间,这里是10秒
const int EPOLL_WAIT_TIME = 10000;

// 创建epoll描述符
Epoller::Epoller() :
    epollfd_(epoll_create1(EPOLL_CLOEXEC)),
    active_event_(EVENTS_NUM)
{
    //assert(epollfd_ > 0);
}

Epoller::~Epoller() {}

// 往epoll的事件表上注册事件
void Epoller::Add(std::shared_ptr<EventBase> eventbase)
{
    // 创建epoll_event结构体作为epoll_ctl的参数
    int fd = eventbase->GetFD();
    struct epoll_event event;
    event.events = eventbase->GetEvents();
    event.data.fd = fd;

    // 添加进映射表
    fd_2_eventbase_list_[fd] = eventbase;
    // 往内核epoll事件表注册
    if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &event) < 0)
    {
        perror("Epoller::Add() error");
        // 注册失败，从映射表中清除
        fd_2_eventbase_list_[fd].reset();
    }
}

// 修改注册的事件
void Epoller::Mod(std::shared_ptr<EventBase> eventbase)
{
    int fd = eventbase->GetFD();
    struct epoll_event event;
    event.events = eventbase->GetEvents();
    event.data.fd = fd;

    if (epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &event) < 0)
    {
        perror("Epoller::Mod() error");
        fd_2_eventbase_list_[fd].reset();
    }
}

// 删除注册的事件
void Epoller::Del(std::shared_ptr<EventBase> eventbase)
{
    int fd = eventbase->GetFD();
    struct epoll_event event;
    event.events = eventbase->GetEvents();
    event.data.fd = fd;

    if (epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, &event) < 0)
    {
        perror("Epoller::Del() error");
    }
    fd_2_eventbase_list_[fd].reset();
}

// 等待事件发生
std::vector<std::shared_ptr<EventBase>> Epoller::Poll()
{
    while (true)
    {
        std::cout << "in Poll" << std::endl; 
        int active_event_count = epoll_wait(epollfd_, &*active_event_.begin(), active_event_.size(), EPOLL_WAIT_TIME);
        std::cout << "after epoll wait()" << std::endl;
        if (active_event_count < 0)
            perror("epoll wait error");

        std::vector<std::shared_ptr<EventBase>> active_eventbase_list;
        for (int i = 0; i < active_event_count; ++i)
        {
            // 从映射表中取出eventbase
            std::shared_ptr<EventBase> eventbase = fd_2_eventbase_list_[active_event_[i].data.fd];
            // 设置eventbase的活跃事件
            eventbase->SetRevents(active_event_[i].events);

            active_eventbase_list.push_back(eventbase);
        }

        // 当有事件发生时则返回
        if (active_eventbase_list.size() > 0)
            return active_eventbase_list;
    }
}