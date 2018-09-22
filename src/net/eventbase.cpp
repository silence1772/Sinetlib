#include "eventbase.h"

EventBase::EventBase(int fd) : 
    fd_(fd),
    events_(0),
    revents_(0)
{}

EventBase::~EventBase() {}

void EventBase::EnableReadEvents()
{
    events_ |= (EPOLLIN | EPOLLPRI);
}

void EventBase::EnableReadEventsET()
{
    events_ |= (EPOLLIN | EPOLLPRI | EPOLLET);
}

void EventBase::EnableWriteEvents()
{
    events_ |= EPOLLOUT;
}

void EventBase::EnableWriteEventsET()
{
    events_ |= (EPOLLOUT | EPOLLET);
}

void EventBase::HandleEvent()
{
    // 挂起事件
    if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN))
    {
        return;
    }
    // 错误事件
    if (revents_ & EPOLLERR)
    {
        if (error_callback_)
            error_callback_();
        return;
    }
    // 可读事件
    if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLHUP))
    {
        if (read_callback_)
            read_callback_();
    }
    // 可写事件
    if (revents_ & EPOLLOUT)
    {
        if (write_callback_)
            write_callback_();
    }
}

