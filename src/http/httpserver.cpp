#include "httpserver.h"
#include "httpparser.h"
#include "httprequest.h"
#include "httpresponse.h"


// 默认处理
void PageNotFoundHandler(const HttpRequest&, std::unordered_map<std::string, std::string>&, HttpResponse* response)
{
    response->SetStatusCode(HttpResponse::NOT_FOUND);
    response->SetStatusMessage("Not Found");
    response->SetCloseConnection(true);
    response->SetContentType("text/html");

    std::string body = "404 page not found";
    response->AddHeader("Content-Length", std::to_string(body.size()));
    response->AppendHeaderToBuffer();
    response->AppendBodyToBuffer(body);
}


HttpServer::HttpServer(Looper* loop, int port, int thread_num) :
    server_(loop, port, thread_num),
    default_handler_(PageNotFoundHandler)
{
    server_.SetConnectionEstablishedCB(std::bind(&HttpServer::OnConnection, this, std::placeholders::_1));
    server_.SetMessageArrivalCB(std::bind(&HttpServer::OnMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    server_.SetConnectionCloseCB(std::bind(&HttpServer::OnClose, this, std::placeholders::_1));
}

void HttpServer::Start()
{
    server_.Start();
}

void HttpServer::OnConnection(const std::shared_ptr<Connection>& conn)
{
    // 建立解析器
    conn->SetContext(HttpParser());
}

void HttpServer::OnMessage(const std::shared_ptr<Connection>& conn, IOBuffer* buf, Timestamp t)
{
    // 取出连接对应的解析器
    HttpParser* parser = any_cast<HttpParser>(conn->GetMutableContext());

    // 无效请求
    if (!parser->ParseRequest(buf, t))
    {
        conn->Send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->Shutdown();
    }

    if (parser->IsGotAll())
    {
        OnRequest(conn, parser->GetRequest());
        // 处理完后重置解析器初始状态
        parser->Reset();
    }
}

void HttpServer::OnRequest(const std::shared_ptr<Connection>& conn, const HttpRequest& request)
{
    // 判断是否为长连接
    const std::string& connection = request.GetHeader("Connection");
    bool close = connection == "close" || (request.GetVersion() == HttpRequest::HTTP10 && connection != "keep-alive");

    HttpResponse response(close);
    
    // 用来提取参数的map
    std::unordered_map<std::string, std::string> match_map;
    // 由路由器进行逐一匹配，找到合适的路由
    auto handler = router_.Match(request, &match_map);
    if (handler != nullptr)
    {
        handler(request, match_map, &response);
    }
    else
    {
        default_handler_(request, match_map, &response);
    }

    // 发送响应内容
    conn->Send(response.GetBuffer());
    // 如果不是长连接就关闭底层的tcp连接
    if (response.GetCloseConnection())
    {
        conn->Shutdown();
    }
}

void HttpServer::OnClose(const std::shared_ptr<Connection>& conn)
{
    // 清理连接对应的解析器
    //parser_map_.erase(conn->GetFd());
}