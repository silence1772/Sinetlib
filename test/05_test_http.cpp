#include "httpserver.h"
#include "looper.h"
#include "iobuffer.h"
#include "currentthread.h"
#include <iostream>
#include "timestamp.h"

void OnHttpCallback(const HttpRequest& request, std::map<std::string, std::string>& match_map, HttpResponse* response)
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

    HttpServer s(&loop, 8888, 3);
    //s.SetHttpCallback(OnCallback);
    s.NewRoute()->
    SetPath("/test/{name:[a-zA-Z]+}/");

    s.NewRoute()
    ->SetPath("/query")
    ->SetQuery("my", "tt")
    ->SetHeader("Connection", "keep-alive")
    ->SetHandler(OnHttpCallback);

    s.NewRoute()
    ->SetPath("/header")
    ->SetHeader("Connection", "keep-alive")
    ->SetHandler(OnHttpCallback);

    s.NewRoute()->
    SetMethod("GET")->
    SetPath("/method")->
    SetHandler(OnHttpCallback);

    s.NewRoute()->
    SetPath("/my/{name}/{xin}");
    
    s.NewRoute()->
    SetPrefix("/file/")->
    SetHandler(s.GetFileHandler("/home/silence/mys/"));

    s.Start();

    loop.Start();
}