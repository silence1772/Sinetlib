#include "httpserver.h"
#include "httpparser.h"
#include "httprequest.h"
#include "httpresponse.h"
#include <iostream>
void PageNotFoundHandler(const HttpRequest&, std::map<std::string, std::string>&, HttpResponse* response)
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
    http_callback_(PageNotFoundHandler)
{
    server_.SetConnectionEstablishedCB(std::bind(&HttpServer::OnConnection, this, std::placeholders::_1));
    server_.SetMessageArrivalCB(std::bind(&HttpServer::OnMessage, this, std::placeholders::_1, std::placeholders::_2));
    server_.SetConnectionCloseCB(std::bind(&HttpServer::OnClose, this, std::placeholders::_1));
}

void HttpServer::Start()
{
    server_.Start();
}

void HttpServer::OnConnection(const std::shared_ptr<Connection>& conn)
{
    //conn->SetContext(HttpParser());
    parser_map_[conn->GetFd()] = std::make_shared<HttpParser>();
}

void HttpServer::OnMessage(const std::shared_ptr<Connection>& conn, IOBuffer* buf)
{
    //HttpParser* parser = any_cast<HttpParser>(conn->GetMutableContext());
    std::shared_ptr<HttpParser> parser = parser_map_[conn->GetFd()];

    if (!parser->ParseRequest(buf))
    {
        conn->Send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->Shutdown();
    }

    if (parser->IsGotAll())
    {
        OnRequest(conn, parser->GetRequest());
        parser->Reset();
    }
}

void HttpServer::OnRequest(const std::shared_ptr<Connection>& conn, const HttpRequest& request)
{
    const std::string& connection = request.GetHeader("Connection");
    bool close = connection == "close" || (request.GetVersion() == HttpRequest::HTTP10 && connection != "Keep-Alive");

    HttpResponse response(close);
    
    std::map<std::string, std::string> match_map;
    auto handler = router_.Match(request, &match_map);
    if (handler != nullptr)
    {
        handler(request, match_map, &response);
        std::cout << "match" << std::endl;
    }
    else
    {
        http_callback_(request, match_map, &response);
    }
    // if (router_.Match(request))
    // {
    //     std::cout << "match" << std::endl;
    // }
    //http_callback_(request, match_map, &response);
    //IOBuffer buffer;
    //response.AppendToBuffer();
    conn->Send(response.GetBuffer());
    if (response.GetCloseConnection())
    {
        conn->Shutdown();
    }
}

void HttpServer::OnClose(const std::shared_ptr<Connection>& conn)
{
    parser_map_.erase(conn->GetFd());
}