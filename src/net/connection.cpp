#include "connection.h"
#include "eventbase.h"
#include "looper.h"
#include "logger.h"
#include "util.h"
#include <unistd.h>

Connection::Connection(Looper* loop, int conn_sockfd, const struct sockaddr_in& local_addr, const struct sockaddr_in& peer_addr) :
    loop_(loop),
    conn_sockfd_(conn_sockfd),
    conn_eventbase_(new EventBase(conn_sockfd_)),
    local_addr_(local_addr),
    peer_addr_(peer_addr)
{
    conn_eventbase_->SetReadCallback(std::bind(&Connection::HandleRead, this, std::placeholders::_1));
    conn_eventbase_->SetWriteCallback(std::bind(&Connection::HandleWrite, this));
    conn_eventbase_->SetCloseCallback(std::bind(&Connection::HandleClose, this));
    conn_eventbase_->EnableReadEvents();
}

Connection::~Connection() {}

// 连接建立后在分配的线程上注册事件
void Connection::Register()
{
    loop_->AddEventBase(conn_eventbase_);
    if (connection_established_cb_)
        connection_established_cb_(shared_from_this());
}

// 往对端发送消息
void Connection::Send(const void* data, size_t len)
{
    ssize_t n_wrote = 0;
    // 当output buffer为空时直接write而不经过缓冲区
    if (!conn_eventbase_->IsWriting() && output_buffer_.GetReadableSize() == 0)
    {
        n_wrote = write(conn_eventbase_->GetFd(), data, len);
        if (n_wrote >= 0)
        {
            // 数据已写完
            if ((size_t)n_wrote == len && reply_complete_cb_)
                reply_complete_cb_(shared_from_this());
        }
        else
        {
            n_wrote = 0;
            if (errno != EWOULDBLOCK)
            {
                LOG_ERROR << "write error";
            }
        }
    }

    // 数据未能一次性写完或者缓冲区不为空
    if ((size_t)n_wrote < len)
    {
        output_buffer_.Append(static_cast<const char*>(data) + n_wrote, len - n_wrote);
        if (!conn_eventbase_->IsWriting())
        {
            conn_eventbase_->EnableWriteEvents();
            loop_->ModEventBase(conn_eventbase_);
        }
    }
}

void Connection::Send(const std::string& message)
{
    Send(message.data(), message.size());
}

void Connection::Send(IOBuffer& buffer)
{
    Send(buffer.GetReadablePtr(), buffer.GetReadableSize());
    buffer.RetrieveAll();
}

void Connection::Shutdown()
{
    util::ShutdownWrite(conn_sockfd_);
}

// 处理可读事件
void Connection::HandleRead(Timestamp t)
{
    int saved_errno = 0;
    ssize_t n = 0;

    n = input_buffer_.ReadFd(conn_eventbase_->GetFd(), &saved_errno);
   
    if (n > 0)
    {
        if (message_arrival_cb_)
            message_arrival_cb_(shared_from_this(), &input_buffer_, t);
    }
    else
    {
        HandleClose();
    }
}

// 处理可写事件
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
                    reply_complete_cb_(shared_from_this());
                }
            }
        }
        else
        {
            LOG_ERROR << "write error";
        }
    }
}

// 关闭连接
void Connection::HandleClose()
{
    loop_->DelEventBase(conn_eventbase_);
    if (connection_close_cb_)
        connection_close_cb_(shared_from_this());
    if (suicide_cb_)
        suicide_cb_(shared_from_this());
}