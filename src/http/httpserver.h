#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "server.h"
#include "router.h"
#include "filehandler.h"
#include "timestamp.h"

class HttpParser;
class HttpRequest;
class HttpResponse;

class HttpServer
{
public:
    HttpServer(Looper* loop, int port, int thread_num = 1);

    void SetDefaultHandler(const Route::Handler& cb) { default_handler_ = cb; }

    Route::RoutePtr NewRoute() { return router_.NewRoute(); }

    void Start();
    
    Route::Handler GetFileHandler(std::string path)
    {
        fh_.SetPrefixPath(path);
        return fh_.GetHandler();
    }

private:
    void OnConnection(const std::shared_ptr<Connection>& conn);
    void OnMessage(const std::shared_ptr<Connection>& conn, IOBuffer* buf, Timestamp t);
    void OnRequest(const std::shared_ptr<Connection>& conn, const HttpRequest& request);
    void OnClose(const std::shared_ptr<Connection>& conn);

    Server server_;
    // 默认处理函数
    Route::Handler default_handler_;
    // 分发路由
    Router router_;
    // 处理对文件的访问
    FileHandler fh_;
};

#endif // HTTP_SERVER_H