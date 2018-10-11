#include "httpserver.h"
#include "httpcontext.h"
#include "httprequest.h"
#include "httpresponse.h"

void DefaultHttpCallback(const HttpRequest&, HttpResponse* response)
{
    response->SetStatusCode(HttpResponse::NOT_FOUND);
    response->SetStatusMessage("Not Found");
    response->SetCloseConnection(true);
}

HttpServer::HttpServer(Looper* loop, int port, int thread_num) :
    server_(loop, port, thread_num),
    http_callback_(DefaultHttpCallback)
{
    server_.SetConnectionEstablishedCB(std::bind(&HttpServer::OnConnection, this, std::placeholders::_1));
    server_.SetMessageArrivalCB(std::bind(&HttpServer::OnMessage, this, std::placeholders::_1, std::placeholders::_2));
}

void HttpServer::Start()
{
    server_.Start();
}

void HttpServer::OnConnection(const std::shared_ptr<Connection>& conn)
{
    conn->SetContext(HttpContext());
}

void HttpServer::OnMessage(const std::shared_ptr<Connection>& conn, IOBuffer* buf)
{
    HttpContext* context = any_cast<HttpContext>(conn->GetMutableContext());

    if (!context->ParseRequest(buf))
    {
        conn->Send("HTTP/1.1 400 Bad Request\r\n\r\n");
    }

    if (context->IsGotAll())
    {
        OnRequest(conn, context->GetRequest());
        context->Reset();
    }
}

void HttpServer::OnRequest(const std::shared_ptr<Connection>& conn, const HttpRequest& request)
{
    const std::string& connection = request.GetHeader("Connection");
    bool close = connection == "close" || (request.GetVersion() == HttpRequest::HTTP10 && connection != "Keep-Alive");

    HttpResponse response(close);
    http_callback_(request, &response);
    IOBuffer buffer;
    response.AppendToBuffer(&buffer);
    conn->Send(buffer);
}