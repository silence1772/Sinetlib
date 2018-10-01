#ifndef SERVER_H
#define SERVER_H

#include "looper.h"
#include "connection.h"
#include <netinet/in.h>
#include <memory>
#include <map>


class ThreadPool;
class EventBase;

class Server
{
public:
    using Callback = std::function<void(const std::shared_ptr<Connection>&)>;

    Server(Looper* loop, int port, int thread_num = 1, bool is_keep_alive_connection = false);
    ~Server();
    
    void Start();

    // 设置回调
    void SetConnectionEstablishedCB(Callback&& cb) { connection_established_cb_ = cb; }
    void SetMessageArrivalCB(Callback&& cb) { message_arrival_cb_ = cb; }
    void SetReplyCompleteCB(Callback&& cb) { reply_complete_cb_ = cb; }

private:
    // 处理新连接
    void HandelNewConnection();

    // 移除连接
    void RemoveConnection4CloseCB(const std::shared_ptr<Connection>& conn);
    void RemoveConnection(int conn_fd);

    Looper* loop_;
    std::unique_ptr<ThreadPool> thread_pool_;

    const int accept_sockfd_;
    struct sockaddr_in addr_;
    std::shared_ptr<EventBase> accept_eventbase_;

    // 描述符到连接的映射，用来保存所有连接
    std::map<int, std::shared_ptr<Connection>> connection_map_;

    // 连接建立后的回调函数
    Callback connection_established_cb_;
    // 新消息到来时
    Callback message_arrival_cb_;
    // 答复消息完成时
    Callback reply_complete_cb_;

    // 是否为保活连接，即长连接
    bool is_keep_alive_connection_;
};

#endif // SERVER_H