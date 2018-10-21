#include "server.h"
#include "util.h"
#include "threadpool.h"
#include "eventbase.h"
#include <string.h> //bzero
#include <netinet/in.h>

Server::Server(Looper* loop, int port, int thread_num) :
    loop_(loop),
    thread_pool_(new ThreadPool(loop_, thread_num)),
    accept_sockfd_(util::Create())
{
    // 创建服务器地址结构
    bzero(&addr_, sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_addr.s_addr = htonl(INADDR_ANY);
    addr_.sin_port = htons(port);

    // 设置可重用及绑定地址结构
    util::SetReuseAddr(accept_sockfd_);
    util::Bind(accept_sockfd_, addr_);
    
    // 初始化事件
    accept_eventbase_ = std::make_shared<EventBase>(accept_sockfd_);
    accept_eventbase_->SetReadCallback(std::bind(&Server::HandelNewConnection, this, std::placeholders::_1));
    accept_eventbase_->EnableReadEvents();
}

Server::~Server()
{
    util::Close(accept_sockfd_);
}

// 启动
void Server::Start()
{
    util::Listen(accept_sockfd_);
    loop_->AddEventBase(accept_eventbase_);
    thread_pool_->Start();
}

// 处理新连接
void Server::HandelNewConnection(Timestamp t)
{
    // 客户端地址结构
    struct sockaddr_in peer_addr;
    bzero(&peer_addr, sizeof(peer_addr));
    int conn_fd = util::Accept(accept_sockfd_, &peer_addr);

    // 从线程池中取用线程给连接使用
    Looper* io_loop = thread_pool_->TakeOutLoop();
    std::shared_ptr<Connection> conn = std::make_shared<Connection>(io_loop, conn_fd, addr_, peer_addr);
    conn->SetConnectionEstablishedCB(connection_established_cb_);
    conn->SetMessageArrivalCB(message_arrival_cb_);
    conn->SetReplyCompleteCB(reply_complete_cb_);
    conn->SetConnectionCloseCB(connection_close_cb_);
    conn->SetSuicideCB(std::bind(&Server::RemoveConnection4CloseCB, this, std::placeholders::_1));

    connection_map_[conn_fd] = conn;

    // 在分配到的线程上注册事件
    io_loop->RunTask(std::bind(&Connection::Register, conn));
}

// 移除连接，由线程池中线程调用
void Server::RemoveConnection4CloseCB(const std::shared_ptr<Connection>& conn)
{

    loop_->AddTask(std::bind(&Server::RemoveConnection, this, conn->GetFd()));
}

// 移除连接，在此线程调用
void Server::RemoveConnection(int conn_fd)
{
    connection_map_.erase(conn_fd);
}