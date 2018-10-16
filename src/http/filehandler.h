#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include "httprequest.h"
#include "httpresponse.h"
#include <functional>
#include <string>

class FileHandler
{
public:
    FileHandler();
    ~FileHandler();

    // 设置映射路径
    void SetPrefixPath(std::string path) { prefix_path_ = path; }
    
    // 获取处理函数
    std::function<void(const HttpRequest&, std::map<std::string, std::string>&, HttpResponse* response)> GetHandler()
    {
        return std::bind(&FileHandler::Handler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    }
private:
    // 处理对于静态文件的访问
    void Handler(const HttpRequest& request, std::map<std::string, std::string>& match_map, HttpResponse* response);

    // 映射的文件目录路径
    std::string prefix_path_;
};

#endif // FILE_HANDLER_H