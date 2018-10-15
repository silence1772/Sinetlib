#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "server.h"
#include "router.h"
#include "filehandler.h"
#include <map>

class HttpParser;
class HttpRequest;
class HttpResponse;

class HttpServer
{
public:
    using HttpCallback = std::function<void(const HttpRequest&, std::map<std::string, std::string>&, HttpResponse*)>;
    HttpServer(Looper* loop, int port, int thread_num = 1);

    void SetHttpCallback(const HttpCallback& cb)
    {
        http_callback_ = cb;
    }

    Router::RoutePtr NewRoute() { return router_.NewRoute(); }

    void Start();
    
    HttpCallback GetFileHandler(std::string path)
    {
        fh_.SetPrefixPath(path);
        return fh_.GetHandler();
    }
private:
    void OnConnection(const std::shared_ptr<Connection>& conn);
    void OnMessage(const std::shared_ptr<Connection>& conn, IOBuffer* buf);
    void OnRequest(const std::shared_ptr<Connection>& conn, const HttpRequest& request);
    void OnClose(const std::shared_ptr<Connection>& conn);

    Server server_;
    HttpCallback http_callback_;

    std::map<int, std::shared_ptr<HttpParser>> parser_map_;

    Router router_;
    FileHandler fh_;
};

#endif // HTTP_SERVER_H