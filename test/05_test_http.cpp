#include "httpserver.h"
#include "looper.h"
#include "iobuffer.h"
#include "currentthread.h"
#include <iostream>
#include "timestamp.h"

void OnHttpCallback(const HttpRequest& request, HttpResponse* response)
{
    // response->SetStatusCode(HttpResponse::NOT_FOUND);
    // response->SetStatusMessage("Not Found");
    // response->SetCloseConnection(true);
}

int main()
{
    Looper loop;

    HttpServer s(&loop, 8888, 3);
    //s.SetHttpCallback(OnCallback);
    s.NewRoute()->
    SetPath("/test/{name:[a-zA-Z]+}/");

    s.NewRoute()->
    SetPath("/my/{name}/");
    
    s.NewRoute()->
    SetPrefix("/file/")->
    SetHandler(s.GetFileHandler("/home/silence/mys/"));

    s.Start();

    loop.Start();
}