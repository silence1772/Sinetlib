#include "eventbase.h"

EventBase::EventBase(int fd)
    : fd_(fd),
      events_(0),
      revents_(0)
{
    
}

EventBase::~EventBase() {}

void EventBase::HandleEvent()
{
    if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN))
    {
        return;
    }
    if (revents_ & EPOLLERR)
    {
        if (error_callback_)
            error_callback_();
        return;
    }
    if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLHUP))
    {
        if (read_callback_)
            read_callback_();
    }
    if (revents_ & EPOLLOUT)
    {
        if (write_callback_)
            write_callback_();
    }
}

