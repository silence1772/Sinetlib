#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "server.h"

class HttpRequest;
class HttpResponse;

class HttpServer
{
public:
    using HttpCallback = std::function<void(const HttpRequest&, HttpResponse*)>;
    HttpServer(Looper* loop, int port, int thread_num = 1);

    void SetHttpCallback(const HttpCallback& cb)
    {
        http_callback_ = cb;
    }

    void Start();
    
private:
    void OnConnection(const std::shared_ptr<Connection>& conn);
    void OnMessage(const std::shared_ptr<Connection>& conn, IOBuffer* buf);
    void OnRequest(const std::shared_ptr<Connection>& conn, const HttpRequest& request);

    Server server_;
    HttpCallback http_callback_;
};

#endif // HTTP_SERVER_H