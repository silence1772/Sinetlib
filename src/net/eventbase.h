#ifndef EVENTBASE_H
#define EVENTBASE_H

#include <functional>
#include <sys/epoll.h>

class EventBase
{
public:
    using Callback = std::function<void()>;

    EventBase(int fd);
    ~EventBase();

    void HandleEvent();

    void SetReadCallback(Callback&& read_callback) { read_callback_ = read_callback; }
    void SetWriteCallback(Callback&& write_callback) { write_callback_ = write_callback; }
    void SetErrorCallback(Callback&& error_callback) { error_callback_ = error_callback; }

    void SetRevents(int revents) { revents_ = revents; }
    void SetReadEvents(int events) { events_ = events; }

    int GetFD() const { return fd_; }
    int GetEvents() const { return events_; }
private:
    const int fd_;
    // 关注的事件
    int events_;
    // 返回的事件
    int revents_;

    Callback read_callback_;
    Callback write_callback_;
    Callback error_callback_;
};

#endif // EVENTBASE_H