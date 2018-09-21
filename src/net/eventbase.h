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

    // 关注可读事件
    void EnableReadEvents();
    // 关注可写事件
    void EnableWriteEvents();

    // 设置相应的事件处理函数
    void SetReadCallback(Callback&& read_callback) { read_callback_ = read_callback; }
    void SetWriteCallback(Callback&& write_callback) { write_callback_ = write_callback; }
    void SetErrorCallback(Callback&& error_callback) { error_callback_ = error_callback; }

    // 设置返回的事件
    void SetRevents(int revents) { revents_ = revents; }
    
    // 分发事件处理
    void HandleEvent();

    int GetFD() const { return fd_; }
    int GetEvents() const { return events_; }
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
};

#endif // EVENTBASE_H