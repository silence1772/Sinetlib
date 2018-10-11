#include "httpserver.h"
#include "looper.h"
#include "iobuffer.h"
#include "currentthread.h"
#include <iostream>
#include "timestamp.h"

void OnCallback(const HttpRequest& request, HttpResponse* response)
{
    
}

int main()
{
    Looper loop;

    HttpServer s(&loop, 8888, 3);
    //s.SetHttpCallback(OnCallback);
    s.Start();

    loop.Start();
}