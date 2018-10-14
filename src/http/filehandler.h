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
    
    std::function<void(const HttpRequest&, HttpResponse* response)> GetHandler()
    {
        return std::bind(&FileHandler::Handler, this, std::placeholders::_1, std::placeholders::_2);
    }
private:
    void Handler(const HttpRequest&, HttpResponse* response)
    {
        std::cout << "handler file " << prefix_path_ << std::endl;
    }

    std::string prefix_path_;
};

#endif // FILE_HANDLER_H