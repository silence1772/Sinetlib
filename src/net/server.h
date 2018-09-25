#ifndef SERVER_H
#define SERVER_H

#include "looper.h"
#include <memory>
#include <netinet/in.h>
#include <map>


class ThreadPool;
class EventBase;
class Connection;

class Server
{
public:
    Server(Looper* loop, int port, int thread_num = 1);
    ~Server();
    
    void Start();

    void SetConnectionEstablishedCB(std::function<void()>&& cb) { connection_established_cb_ = cb; }
    void SetMessageArrivalCB(std::function<void()>&& cb) { message_arrival_cb_ = cb; }
    void SetReplyCompleteCB(std::function<void()>&& cb) { reply_complete_cb_ = cb; }
    void SetConnectionCloseCB(std::function<void()>&& cb) { connection_close_cb_ = cb; }
private:
    void HandelNewConnection();

    void RemoveConnection4CloseCB(int conn_fd);
    void RemoveConnection(int conn_fd);

    Looper* loop_;
    std::unique_ptr<ThreadPool> thread_pool_;

    const int accept_sockfd_;
    struct sockaddr_in addr_;
    std::shared_ptr<EventBase> accept_eventbase_;

    std::map<int, std::shared_ptr<Connection>> connection_map_;

    // 供用户设置的tcp连接各个状态的回调函数
    // 连接建立后的回调函数
    std::function<void()> connection_established_cb_;
    // 新消息到来时回调
    std::function<void()> message_arrival_cb_;
    // 答复消息完成时回调
    std::function<void()> reply_complete_cb_;
    // 连接关闭时回调
    std::function<void()> connection_close_cb_;
};

#endif // SERVER_H