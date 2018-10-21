#ifndef UTIL_H
#define UTIL_H

#include <arpa/inet.h>
#include "timestamp.h"
#include <string>

namespace util
{
    
// socket
int Create();
int Connect(int sockfd, const struct sockaddr_in& addr);
void Bind(int sockfd, const struct sockaddr_in& addr);
void Listen(int sockfd);
int Accept(int sockfd, struct sockaddr_in* addr);
void Close(int sockfd);
void ShutdownWrite(int sockfd);
void SetReuseAddr(int sockfd);

// time
std::string ToFormatLocalTime(Timestamp time);
std::string ToFormatHttpGMTTime(Timestamp time);
Timestamp TimespecToTimestamp(struct timespec& ts);

// string
void ToUpper(std::string &str);
void ToLower(std::string &str);

}

#endif // UTIL_H