#include "eventbase.h"
#include <iostream>

EventBase::EventBase(int fd) : 
    fd_(fd),
    events_(0),
    revents_(0)
{}

EventBase::~EventBase() {}

void EventBase::HandleEvent()
{
    // 服务器端发生异常
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
    // 客户端关闭连接
    if (revents_ & EPOLLRDHUP)
    {
        if (close_callback_)
            close_callback_();
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

