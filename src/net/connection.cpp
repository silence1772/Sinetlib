#include "connection.h"
#include "eventbase.h"
#include "looper.h"
#include <iostream>
#include <unistd.h>

Connection::Connection(Looper* loop, int conn_sockfd, bool is_keep_alive_connection) :
    loop_(loop),
    conn_sockfd_(conn_sockfd),
    conn_eventbase_(new EventBase(conn_sockfd_)),
    is_keep_alive_connection_(is_keep_alive_connection)
{
    conn_eventbase_->SetReadCallback(std::bind(&Connection::HandleRead, this));
    conn_eventbase_->SetWriteCallback(std::bind(&Connection::HandleWrite, this));
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

void Connection::Send(const std::string& message)
{
    ssize_t n_wrote = 0;
    // writing directly when output buffer is empty
    if (!conn_eventbase_->IsWriting() && output_buffer_.GetReadableSize() == 0)
    {
        n_wrote = write(conn_eventbase_->GetFd(), message.data(), message.size());
        if (n_wrote >= 0)
        {
            // if ((size_t)n_wrote < message.size())
            // {
            //     LOG_TRACE << "need to write again";
            // }
            // else if (write_complete_callback_)
            // {
            //     loop_->QueueInLoop(std::bind(write_complete_callback_, shared_from_this()));
            // }
            if ((size_t)n_wrote == message.size() && reply_complete_cb_)
                reply_complete_cb_();
        }
        else
        {
            n_wrote = 0;
            // if (errno != EWOULDBLOCK)
            // {
            //     LOG_SYSERR << "write error";
            // }
        }
    }

    if ((size_t)n_wrote < message.size())
    {
        output_buffer_.Append(message.data() + n_wrote, message.size() - n_wrote);
        if (!conn_eventbase_->IsWriting())
        {
            conn_eventbase_->EnableWriteEvents();
            loop_->ModEventBase(conn_eventbase_);
        }
    }
}

void Connection::HandleRead()
{
    int saved_errno = 0;
    ssize_t n = 0;
    if (is_keep_alive_connection_)
    {
        n = input_buffer_.ReadFdOnce(conn_eventbase_->GetFd(), &saved_errno);
    }
    else
    {
        n = input_buffer_.ReadFdRepeatedly(conn_eventbase_->GetFd(), &saved_errno);
    }
   
    if (n > 0)
    {
        std::cout << saved_errno << std::endl;
        if (message_arrival_cb_)
            message_arrival_cb_(shared_from_this());
    }
    else if (n == 0)
    {
        HandleClose();
    }
    else
    {
        HandleClose();
    }
}

void Connection::HandleWrite()
{
    if (conn_eventbase_->IsWriting())
    {
        ssize_t n = write(conn_eventbase_->GetFd(), output_buffer_.GetReadablePtr(), output_buffer_.GetReadableSize());
        if (n > 0)
        {
            output_buffer_.Retrieve(n);
            if (output_buffer_.GetReadableSize() == 0)
            {
                conn_eventbase_->DisableWriteEvents();
                loop_->ModEventBase(conn_eventbase_);
                if (reply_complete_cb_)
                {
                    reply_complete_cb_();
                }
                // if (state_ == Disconnecting)
                // {
                //     ShutdownInLoop();
                // }
            }
            // else
            // {
            //     LOG_TRACE << "need to write again";
            // }
        }
        // else
        // {
        //     LOG_SYSERR << "write error";
        //     abort();
        // }
    }
    // else
    // {
    //     LOG_TRACE << "Connection is down, no more writing";
    // }
}

void Connection::HandleClose()
{
    loop_->DelEventBase(conn_eventbase_);
    if (connection_close_cb_)
        connection_close_cb_(conn_sockfd_);
}