#include "connection.h"
#include "eventbase.h"
#include "looper.h"
#include <iostream>

Connection::Connection(Looper* loop, int conn_sockfd, bool is_keep_alive_connection) :
    loop_(loop),
    conn_sockfd_(conn_sockfd),
    conn_eventbase_(new EventBase(conn_sockfd_)),
    is_keep_alive_connection_(is_keep_alive_connection)
{
    conn_eventbase_->SetReadCallback(std::bind(&Connection::HandleRead, this));
    conn_eventbase_->SetCloseCallback(std::bind(&Connection::HandleClose, this));
    conn_eventbase_->EnableEdgeTriggered();
    conn_eventbase_->EnableReadEvents();
    conn_eventbase_->EnableCloseEvents();
}

Connection::~Connection() {}

void Connection::Register()
{
    loop_->AddEventBase(conn_eventbase_);
    if (connection_established_cb_)
        connection_established_cb_();
}

void Connection::HandleRead()
{
    int saved_errno = 0;
    ssize_t n = input_buffer_.ReadFdRepeatedly(conn_eventbase_->GetFD(), &saved_errno);
    if (n > 0)
    {
        std::cout << saved_errno << std::endl;
        if (message_arrival_cb_)
            message_arrival_cb_();
    }
    else if (n == 0)
    {
        std::cout << "n == 0" << std::endl;
    }
    else
    {
        std::cout << "n < 0" << std::endl;
    }
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