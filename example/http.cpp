#include "httpserver.h"

void OnHttpCallback(const HttpRequest& request, std::unordered_map<std::string, std::string>& match_map, HttpResponse* response)
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
    ->SetHeader("Connection", "keep-alive")
    ->SetHandler(OnHttpCallback);
    
    s.NewRoute()
    ->SetPrefix("/file/")
    ->SetHandler(s.GetFileHandler("/home/"));

    s.Start();
    loop.Start();
}