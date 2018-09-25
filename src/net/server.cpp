#include "server.h"
#include "util.h"
#include <string.h> //bzero
#include <netinet/in.h>
#include "threadpool.h"
#include "eventbase.h"
#include "connection.h"
#include <iostream>

Server::Server(Looper* loop, int port, int thread_num) :
    loop_(loop),
    thread_pool_(new ThreadPool(loop_, thread_num)),
    accept_sockfd_(util::Create())
{
    bzero(&addr_, sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_addr.s_addr = htonl(INADDR_ANY);
    addr_.sin_port = htons(port);

    // set reuseaddr
    util::SetReuseAddr(accept_sockfd_);
    util::Bind(accept_sockfd_, addr_);
    
    accept_eventbase_ = std::make_shared<EventBase>(accept_sockfd_);
    accept_eventbase_->SetReadCallback(std::bind(&Server::HandelNewConnection, this));
    accept_eventbase_->EnableEdgeTriggered();
    accept_eventbase_->EnableReadEvents();
}

Server::~Server()
{
    util::Close(accept_sockfd_);
}

void Server::Start()
{
    util::Listen(accept_sockfd_);
    loop_->AddEventBase(accept_eventbase_);
    thread_pool_->Start();
}

void Server::HandelNewConnection()
{
    std::cout << "new connection" << std::endl;
    struct sockaddr_in peer_addr;
    bzero(&peer_addr, sizeof(peer_addr));
    int conn_fd = util::Accept(accept_sockfd_, &peer_addr);

    Looper* io_loop = thread_pool_->TakeOutLoop();
    std::shared_ptr<Connection> conn = std::make_shared<Connection>(io_loop, conn_fd, false);
    conn->SetConnectionEstablishedCB(connection_established_cb_);
    conn->SetMessageArrivalCB(message_arrival_cb_);
    conn->SetReplyCompleteCB(reply_complete_cb_);
    conn->SetConnectionCloseCB(std::bind(&Server::RemoveConnection4CloseCB, this, std::placeholders::_1));

    connection_map_[conn_fd] = conn;

    io_loop->RunTask(std::bind(&Connection::Register, conn));
}

void Server::RemoveConnection4CloseCB(int conn_fd)
{
    loop_->AddTask(std::bind(&Server::RemoveConnection, this, conn_fd));
}

void Server::RemoveConnection(int conn_fd)
{
    connection_map_.erase(conn_fd);
}