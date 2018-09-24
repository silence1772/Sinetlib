#include "connection.h"
#include "eventbase.h"
#include "looper.h"
#include <iostream>

Connection::Connection(Looper* loop, int conn_sockfd) :
    loop_(loop),
    conn_sockfd_(conn_sockfd),
    conn_eventbase_(new EventBase(conn_sockfd_))
{
    conn_eventbase_->SetReadCallback(std::bind(&Connection::HandleRead, this));
    conn_eventbase_->SetCloseCallback(std::bind(&Connection::HandleClose, this));
    conn_eventbase_->EnableEdgeTriggered();
    conn_eventbase_->EnableReadEvents();
    conn_eventbase_->EnableCloseEvents();
}

Connection::~Connection() 
{
    std::cout << "Oh~ I'm done!" << std::endl;
}

void Connection::HandleRead()
{
    std::cout << "HandleRead" << std::endl;
    //loop_->DelEventBase(conn_eventbase_);
}

void Connection::HandleWrite()
{

}

void Connection::HandleClose()
{
    loop_->DelEventBase(conn_eventbase_);
    if (connection_close_cb_)
        connection_close_cb_(conn_sockfd_);
}

void Connection::ConnectEstablished()
{
    loop_->AddEventBase(conn_eventbase_);
    if (connection_established_cb_)
        connection_established_cb_();
}