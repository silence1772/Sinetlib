#ifndef CONNNECTION_H
#define CONNNECTION_H

#include <functional>
#include <memory>
#include "iobuffer.h"

class EventBase;
class Looper;

class Connection : public std::enable_shared_from_this<Connection>
{
public:
    Connection(Looper* loop, int conn_sockfd, bool is_keep_alive_connection);
    ~Connection();

    void Register();
    void Send(const std::string& message);

    void HandleRead();
    void HandleWrite();
    void HandleClose();

    void SetConnectionEstablishedCB(const std::function<void()>& cb) { connection_established_cb_ = cb; }
    void SetMessageArrivalCB(const std::function<void(const std::shared_ptr<Connection>&)>& cb) { message_arrival_cb_ = cb; }
    void SetReplyCompleteCB(const std::function<void()>& cb) { reply_complete_cb_ = cb; }
    void SetConnectionCloseCB(const std::function<void(int)>& cb) { connection_close_cb_ = cb; }

private:
    Looper* loop_;

    const int conn_sockfd_;
    std::shared_ptr<EventBase> conn_eventbase_;
    //struct sockaddr_in local_addr_;
    //struct sockaddr_in peer_addr_;

    std::function<void()> connection_established_cb_;
    std::function<void(const std::shared_ptr<Connection>&)> message_arrival_cb_;
    std::function<void()> reply_complete_cb_;
    std::function<void(int)> connection_close_cb_;

    IOBuffer input_buffer_;
    IOBuffer output_buffer_;

    bool is_keep_alive_connection_;
};

#endif // CONNNECTION_H