#include "server.h"
#include "util.h"
#include <string.h> //bzero
#include <netinet/in.h>
#include "threadpool.h"
#include "eventbase.h"
#include <iostream>

Server::Server(Looper* loop, int port, int thread_num) :
    loop_(loop),
    thread_pool_(new ThreadPool(loop_, thread_num)),
    port_(port),
    accept_sockfd_(util::Create())
{
    bzero(&addr_, sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_addr.s_addr = htonl(INADDR_ANY);
    addr_.sin_port = htons(port_);

    // set reuseaddr
    int optval = 1;
    setsockopt(accept_sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    util::Bind(accept_sockfd_, addr_);
    
    accept_eventbase_ = std::make_shared<EventBase>(accept_sockfd_);
    accept_eventbase_->SetReadCallback(std::bind(&Server::HandelNewConnection, this));
    accept_eventbase_->EnableReadEventsET();
}

Server::~Server() {}

void Server::Start()
{
    util::Listen(accept_sockfd_);
    loop_->AddEventBase(accept_eventbase_);
    thread_pool_->Start();
}

void Server::HandelNewConnection()
{
    std::cout << "new connection" << std::endl;
}