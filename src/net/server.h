#ifndef SERVER_H
#define SERVER_H

#include "looper.h"
#include <memory>
#include <netinet/in.h>

class ThreadPool;
class EventBase;

class Server
{
public:
    Server(Looper* loop, int port, int thread_num = 1);
    ~Server();
    void Start();
    void HandelNewConnection();
    
private:
    Looper* loop_;
    std::unique_ptr<ThreadPool> thread_pool_;

    const int port_;
    struct sockaddr_in addr_;

    const int accept_sockfd_;
    std::shared_ptr<EventBase> accept_eventbase_;
};

#endif // SERVER_H