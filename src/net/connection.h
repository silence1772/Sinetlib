#ifndef CONNNECTION_H
#define CONNNECTION_H

#include "iobuffer.h"
#include <functional>
#include <memory>

class EventBase;
class Looper;

class Connection : public std::enable_shared_from_this<Connection>
{
public:
    using Callback = std::function<void(const std::shared_ptr<Connection>&)>;

    Connection(Looper* loop, int conn_sockfd, const struct sockaddr_in& local_addr, const struct sockaddr_in& peer_addr, bool is_keep_alive_connection);
    ~Connection();

    // 在loop上注册事件，连接建立时调用
    void Register();

    void Send(const std::string& message);

    // 处理事件
    void HandleRead();
    void HandleWrite();
    void HandleClose();

    // 设置回调
    void SetConnectionEstablishedCB(const Callback& cb) { connection_established_cb_ = cb; }
    void SetMessageArrivalCB(const Callback& cb) { message_arrival_cb_ = cb; }
    void SetReplyCompleteCB(const Callback& cb) { reply_complete_cb_ = cb; }
    void SetConnectionCloseCB(const Callback& cb) { connection_close_cb_ = cb; }

    const int GetFd() const { return conn_sockfd_; }
    const IOBuffer& GetInputBuffer() const { return input_buffer_; }
    const IOBuffer& GetOutputBuffer() const { return output_buffer_; }
private:
    Looper* loop_;

    // 连接描述符
    const int conn_sockfd_;
    std::shared_ptr<EventBase> conn_eventbase_;

    // 服务器、客户端地址结构
    struct sockaddr_in local_addr_;
    struct sockaddr_in peer_addr_;

    // 连接建立回调
    Callback connection_established_cb_;
    // 消息到达
    Callback message_arrival_cb_;
    // 答复完成
    Callback reply_complete_cb_;
    // 连接关闭
    Callback connection_close_cb_;

    // 输入输出缓冲区
    IOBuffer input_buffer_;
    IOBuffer output_buffer_;

    // 是否为保活连接，即长连接
    bool is_keep_alive_connection_;
};

#endif // CONNNECTION_H