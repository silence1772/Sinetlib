#ifndef UTIL_H
#define UTIL_H

#include <arpa/inet.h>

namespace util
{

int Create();
int Connect(int sockfd, const struct sockaddr_in& addr);
void Bind(int sockfd, const struct sockaddr_in& addr);
void Listen(int sockfd);
int Accept(int sockfd, struct sockaddr_in* addr);
void Close(int sockfd);
void ShutdownWrite(int sockfd);
void SetReuseAddr(int sockfd);

}

#endif // UTIL_H