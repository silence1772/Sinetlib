#include "epoller.h"
#include <stdio.h>
#include "eventbase.h"
#include <iostream>


const int EVENTS_NUM = 4096;
const int EPOLL_WAIT_TIME = 2000;

Epoller::Epoller() :
    epollfd_(epoll_create1(EPOLL_CLOEXEC)),
    active_event_(EVENTS_NUM)
{
    //assert(epollfd_ > 0);
}

Epoller::~Epoller() {}

void Epoller::Add(std::shared_ptr<EventBase> eventbase)
{
    int fd = eventbase->GetFD();

    struct epoll_event event;
    event.events = eventbase->GetEvents();
    event.data.fd = fd;

    fd_2_eventbase_list_[fd] = eventbase;
    if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &event) < 0)
    {
        perror("Epoller::Add() error");
        fd_2_eventbase_list_[fd].reset();
    }
}

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
            int fd = active_event_[i].data.fd;

            std::shared_ptr<EventBase> eventbase = fd_2_eventbase_list_[fd];

            eventbase->SetRevents(active_event_[i].events);
            active_eventbase_list.push_back(eventbase);
        }

        if (active_eventbase_list.size() > 0)
            return active_eventbase_list;
    }
}