#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include "httprequest.h"
#include "httpresponse.h"
#include <functional>
#include <string>
#include <iostream>
class FileHandler
{
public:
    FileHandler();
    ~FileHandler();

    void SetPrefixPath(std::string path)
    {
        prefix_path_ = path;
    }
    
    std::function<void(const HttpRequest&, std::map<std::string, std::string>&, HttpResponse* response)> GetHandler()
    {
        return std::bind(&FileHandler::Handler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    }
private:
    void Handler(const HttpRequest& request, std::map<std::string, std::string>& match_map, HttpResponse* response);

    std::string prefix_path_;
};

#endif // FILE_HANDLER_H