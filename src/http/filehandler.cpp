#include "filehandler.h"
#include "file.h"
#include "util.h"
#include <iostream>

FileHandler::FileHandler() {}
FileHandler::~FileHandler() {}

void FileHandler::Handler(const HttpRequest& request, std::map<std::string, std::string>& match_map, HttpResponse* response)
{
    // 映射的目录路径加上url中的路径等于实际路径
    File file(prefix_path_ + match_map["file_path"]);
    if (file.Exist())
    {
        if (file.IsFile())
        {
            response->SetStatusCode(HttpResponse::OK);
            response->SetStatusMessage("OK");
            // 设置内容类型及长度
            response->SetContentType(file.GetMimeType());
            response->AddHeader("Content-Length", std::to_string(file.GetSize()));
        
            // 将response头部部分先添加进缓冲区
            response->AppendHeaderToBuffer();
            // 将文件内容作为剩余的body部分添加进缓冲区
            file.ReadToBuffer(response->GetBuffer());
        }
        // 列出目录下的内容
        else
        {
            response->SetStatusCode(HttpResponse::OK);
            response->SetStatusMessage("OK");
            response->SetContentType("text/html");

            // 拼接成如下html格式
            // <pre>
            // <a href="aa/">aa/</a>
            // <a href="bb/">bb/</a>
            // <a href="cc.c">cc.c</a>
            // </pre>
            std::string body = "<pre>\n";
            std::string slash = "/";
            auto dir = file.ListDir();
            for (auto& i : dir)
            {
                slash = i.IsDir() ? "/" : "";
                body += "<a href=\"" + i.GetName() + slash + "\">" + i.GetName() + slash + "</a>\n";
            }
            body += "</pre>";
            // 设置body长度
            response->AddHeader("Content-Length", std::to_string(body.size()));

            response->AppendHeaderToBuffer();
            response->AppendBodyToBuffer(body);
        }
    }
    else
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
}