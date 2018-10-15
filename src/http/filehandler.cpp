#include "filehandler.h"
#include "file.h"

FileHandler::FileHandler()
{}

FileHandler::~FileHandler()
{}

void FileHandler::Handler(const HttpRequest& request, std::map<std::string, std::string>& match_map, HttpResponse* response)
{
    std::cout << prefix_path_ << " " << request.GetPath()  << std::endl;
    std::cout << prefix_path_  << match_map["file_path"] << std::endl;

    File file(prefix_path_ + match_map["file_path"]);
    if (file.Exist())
    {
        std::cout << "true" << std::endl;
        std::cout << file.ReadAsText() << std::endl;
    }
}