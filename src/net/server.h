#ifndef SERVER_H
#define SERVER_H

#include "looper.h"
#include "connection.h"
#include "iobuffer.h"
#include "timestamp.h"
#include <netinet/in.h>
#include <memory>
#include <map>


class ThreadPool;
class EventBase;

class Server
{
public:
    Server(Looper* loop, int port, int thread_num = 1);
    ~Server();
    
    void Start();

    // 设置回调
    void SetConnectionEstablishedCB(Connection::Callback&& cb) { connection_established_cb_ = cb; }
    void SetMessageArrivalCB(Connection::MessageCallback&& cb) { message_arrival_cb_ = cb; }
    void SetReplyCompleteCB(Connection::Callback&& cb) { reply_complete_cb_ = cb; }
    void SetConnectionCloseCB(Connection::Callback&& cb) { connection_close_cb_ = cb; }

private:
    // 处理新连接
    void HandelNewConnection(Timestamp t);

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
    Connection::Callback connection_established_cb_;
    // 新消息到来时
    Connection::MessageCallback message_arrival_cb_;
    // 答复消息完成时
    Connection::Callback reply_complete_cb_;
    Connection::Callback connection_close_cb_;
};

#endif // SERVER_H