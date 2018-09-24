#ifndef CONNNECTION_H
#define CONNNECTION_H

#include <functional>
#include <memory>

class EventBase;
class Looper;

class Connection
{
public:
    Connection(Looper* loop, int conn_sockfd);
    ~Connection();

    void HandleRead();
    void HandleWrite();
    void HandleClose();

    void SetConnectionEstablishedCB(std::function<void()>&& cb) { connection_established_cb_ = cb; }
    void SetMessageArrivalCB(std::function<void()>&& cb) { message_arrival_cb_ = cb; }
    void SetReplyCompleteCB(std::function<void()>&& cb) { reply_complete_cb_ = cb; }
    void SetConnectionCloseCB(std::function<void(int)>&& cb) { connection_close_cb_ = cb; }

    void ConnectEstablished();
private:
    Looper* loop_;

    const int conn_sockfd_;
    std::shared_ptr<EventBase> conn_eventbase_;
    //struct sockaddr_in local_addr_;
    //struct sockaddr_in peer_addr_;

    std::function<void()> connection_established_cb_;
    std::function<void()> message_arrival_cb_;
    std::function<void()> reply_complete_cb_;
    std::function<void(int)> connection_close_cb_;
};

#endif // CONNNECTION_H