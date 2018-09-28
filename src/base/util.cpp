#include "util.h"
#include <sys/socket.h>
#include <unistd.h>

int util::Create()
{
    int sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if (sockfd < 0)
    {
        //LOG_SYSFATAL << "Create error";
    }
    return sockfd;
}

int util::Connect(int sockfd, const struct sockaddr_in& addr)
{
    return connect(sockfd, (struct sockaddr*)&addr, sizeof(addr));
}

void util::Bind(int sockfd, const struct sockaddr_in& addr)
{
    int ret = bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    if (ret < 0)
    {
        //LOG_SYSFATAL << "Bind error";
    }
}

void util::Listen(int sockfd)
{
    int ret = listen(sockfd, SOMAXCONN);
    if (ret < 0)
    {
        //LOG_SYSFATAL << "Listen error";
    }
}

int util::Accept(int sockfd, struct sockaddr_in* addr)
{
    socklen_t addrlen = sizeof(*addr);
    int connfd = accept4(sockfd, (struct sockaddr*)addr, &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (connfd < 0)
    {
        // int saved_errno = errno;
        // LOG_SYSERR << "Accept error";
        // switch (saved_errno)
        // {
        //     case EAGAIN:
        //     case ECONNABORTED:
        //     case EINTR:
        //     case EPROTO: 
        //     case EPERM:
        //     case EMFILE: 
        //         errno = saved_errno;
        //         break;
        //     case EBADF:
        //     case EFAULT:
        //     case EINVAL:
        //     case ENFILE:
        //     case ENOBUFS:
        //     case ENOMEM:
        //     case ENOTSOCK:
        //     case EOPNOTSUPP:
        //         LOG_FATAL << "unexpected error of Accept " << saved_errno;
        //         break;
        //     default:
        //         LOG_FATAL << "unknown error of Accept " << saved_errno;
        //         break;
        // }
    }
    return connfd;
}

void util::Close(int sockfd)
{
    if (close(sockfd) < 0)
    {
        //LOG_SYSERR << "Close error";
    }
}

void util::SetReuseAddr(int sockfd)
{
    int optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}