# Sinetlib: A High Perfermance C++ Network Library

[![Build Status](https://travis-ci.org/silence1772/Sinetlib.svg?branch=master)](https://travis-ci.org/silence1772/Sinetlib)
[![license](https://img.shields.io/github/license/mashape/apistatus.svg)](https://opensource.org/licenses/MIT)

## Introduction
Sinetlib是一个仿照Muduo实现的基于Reactor模式的多线程网络库，附有异步日志，要求Linux 2.6以上内核版本。同时内嵌一个简洁HTTP服务器，可实现路由分发及静态资源访问。

## Feature
* 底层使用Epoll LT模式实现I/O复用，非阻塞I/O
* 多线程、定时器依赖于c++11提供的std::thread、std::chrono库
* Reactor模式，主线程accept请求后，使用Round Robin分发给线程池中线程处理
* 基于小根堆的定时器管理队列
* 双缓冲技术实现的异步日志
* 使用智能指针及RAII机制管理对象生命期
* 使用状态机解析HTTP请求，支持HTTP长连接
* HTTP服务器支持URL路由分发及访问静态资源，可实现RESTful架构

## Build
    $ git clone git@github.com:silence1772/Sinetlib.git
    $ cd Sinetlib
    $ ./build.sh

执行上述脚本前需先安装cmake：

    $ sudo apt-get update
    $ sudo apt-get install cmake

## Usage
### net
Sinetlib的使用十分简单，用户只需设置四个回调即可。四个回调抽象的是TCP连接建立后、有消息到达、答复消息完成、连接关闭这四个状态，用户可以设置各个状态对应的执行函数。

在此之前，用户需要先创建Looper和Server，Server的第二和第三个参数分别是监听的端口号和线程池中线程数，一般情况下建议线程数与CPU核心数接近，以最大程度发挥多线程性能。
```c++
#include "server.h"
#include <iostream>

void OnConnection(const std::shared_ptr<Connection>& conn)
{
    std::cout << "OnConnection" << std::endl;
}
void OnMessage(const std::shared_ptr<Connection>& conn, IOBuffer* buf, Timestamp t)
{
    std::cout << "OnMessage" << std::endl;
}
void OnReply(const std::shared_ptr<Connection>& conn)
{
    std::cout << "OnReply" << std::endl;
}
void OnClose(const std::shared_ptr<Connection>& conn)
{
    std::cout << "OnClose" << std::endl;
}

int main()
{
    Looper loop;
    Server s(&loop, 8888, 4);

    s.SetConnectionEstablishedCB(OnConnection);
    s.SetMessageArrivalCB(OnMessage);
    s.SetReplyCompleteCB(OnReply);
    s.SetConnectionCloseCB(OnClose);

    s.Start();
    loop.Start();
}
```

在提供给用户设置的各个回调函数中，都有一个指向当前连接的指针，在消息到达回调中还暴露了保存接收到的消息的IOBuffer缓冲区指针。

用户可以对这两个指针进行操作,通过读取缓冲区和调用Connection::Send()实现消息的收发。

Connection类：
```c++
// 发送数据到连接的对端
void Send(const void* data, size_t len);
void Send(const std::string& message);
void Send(IOBuffer& buffer);
// 关闭连接中自己的这一端，这会激发TCP的四次挥手进而关闭整个连接
void Shutdown();
// 获取连接描述符
const int GetFd()
// 获取输入输出缓冲区
const IOBuffer& GetInputBuffer()
const IOBuffer& GetOutputBuffer()
```

IOBuffer:
```c++
// 获取可读、可写、预留区的大小
size_t GetReadableSize()
size_t GetWritableSize()
size_t GetPrependSize()
// 获取可读、可写区的首指针
const char* GetReadablePtr()
const char* GetWritablePtr()
// 寻找回车换行符/r/n
const char* FindCRLF()
const char* FindCRLF(const char* start_ptr)
// 寻找换行符/n
const char* FindEOL()
const char* FindEOL(const char* start_ptr)
// 向后移动可读区指针到实际位置
void Retrieve(size_t len)
// 移动可读区指针到指定位置
void RetrieveUntil(const char* end)
// 将可读、可写区指针均移到初始位置，即重置缓冲区
void RetrieveAll()
// 添加数据进缓冲区
void Append(const char* data, size_t len)
void Append(const std::string& str)
// 添加数据进预留区
void Prepend(const void* data, size_t len)
```

### http
Sinetlib内嵌了一个HTTP服务器，设计上借鉴了golang的mux包实现路由分发的思想，同时还支持静态资源访问。

用户可以根据需要设置路由及匹配条件，目前支持URL、请求参数、请求头及请求方法的匹配，并且可以从中提取出参数。
一个请求必须满足所有条件才能匹配这个路由，得到它的Handler。

首先要设置相应的路由处理函数,该函数由用户实现，如果要实现静态资源访问，则需使用HttpServer的文件处理函数
```
Route::SetHandler(YourHandler);
Route::SetHandler(HttpServer::GetFileHandler("/home/mys/"));
```
可以对url进行匹配,并且可以设置正则表达式匹配规则，比如下面第一句匹配对于/path/xxx的访问，并且可以通过key来获取到xxx，此处限定了key必须为字母。
也可以不设置正则匹配条件，如第二句的key可以匹配任何字符。
```
Route::SetPath("/path/{key:[a-zA-Z]+}");
Route::SetPath("/path/{key}");
```
匹配方法头、头部字段、参数：
```
Route::SetMethod("GET");
Route::SetHeader("Header-Name", "Header-Value");
Route::SetQuery("Filed", "Value");
```
匹配前缀：
```
Route::SetPrefix("/file/");
```


完整的程序使用如下,该程序有两个路由，其中第二个为静态资源服务。

该程序可匹配下面两个HTTP请求样例

    GET /path/myname?query=t HTTP/1.1
    Connection: keep-alive

    GET /file/test.jpg HTTP/1.1
第二个请求对应访问位于/home/mys/test.jpg的文件
```c++
#include "httpserver.h"

void MyHandler(const HttpRequest& request, std::unordered_map<std::string, std::string>& match_map, HttpResponse* response)
{
    response->SetStatusCode(HttpResponse::OK);
    response->SetStatusMessage("OK");
    response->SetContentType("text/html");

    std::string body = "temp test page";
    response->AddHeader("Content-Length", std::to_string(body.size()));
    response->AppendHeaderToBuffer();
    response->AppendBodyToBuffer(body);
}

int main()
{
    Looper loop;
    HttpServer s(&loop, 8888, 4);

    s.NewRoute()
    ->SetPath("/path/{name:[a-zA-Z]+}")
    ->SetQuery("query", "t")
    ->SetHeader("Connection", "keep-alive")
    ->SetHandler(MyHandler);
    
    s.NewRoute()
    ->SetPrefix("/file/")
    ->SetHandler(s.GetFileHandler("/home/mys/"));

    s.Start();
    loop.Start();
}
```
