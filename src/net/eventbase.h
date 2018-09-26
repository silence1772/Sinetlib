#ifndef EVENTBASE_H
#define EVENTBASE_H

#include <sys/epoll.h>
#include <functional>

// 事件基类，内含epoll_event的内容及相应事件处理函数
class EventBase
{
public:
    // c++11用法，类似typedef
    using Callback = std::function<void()>;

    EventBase(int fd);
    ~EventBase();

    void EnableEdgeTriggered() { events_ |= EPOLLET; }
    void EnableReadEvents() { events_ |= (EPOLLIN | EPOLLPRI); }
    void EnableWriteEvents() { events_ |= EPOLLOUT; }
    void EnableCloseEvents() { events_ |= EPOLLRDHUP; }

    void DisableWriteEvents() { events_ &= ~EPOLLOUT; }

    // 设置相应的事件处理函数
    void SetReadCallback(Callback&& cb) { read_callback_ = cb; }
    void SetWriteCallback(Callback&& cb) { write_callback_ = cb; }
    void SetErrorCallback(Callback&& cb) { error_callback_ = cb; }
    void SetCloseCallback(Callback&& cb) { close_callback_ = cb; }

    // 设置返回的事件
    void SetRevents(int revents) { revents_ = revents; }
    
    // 分发事件处理
    void HandleEvent();

    int GetFd() const { return fd_; }
    int GetEvents() const { return events_; }

    bool IsWriting() const { return events_ & EPOLLOUT; }
private:
    // 以下两个变量对应epoll_event结构体中内容
    // 文件描述符
    const int fd_;
    // 关注的事件
    int events_;

    // 返回的事件
    int revents_;

    // 事件处理函数
    Callback read_callback_;
    Callback write_callback_;
    Callback error_callback_;
    Callback close_callback_;
};

#endif // EVENTBASE_H