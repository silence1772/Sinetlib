#include "file.h"
#include <fstream>

File::File(std::string path) :
    path_(path)
{
    if (path_[path_.size() - 1] == '/')
        path_.pop_back();
    exist_ = (stat(path_.c_str(), &file_stat_) == 0);
}

bool File::Exist()
{
    return exist_;
}

bool File::IsFile()
{
    if (!exist_)
        return false;
    return S_ISREG(file_stat_.st_mode);
}

bool File::IsDir()
{
    if (!exist_)
        return false;
    return S_ISDIR(file_stat_.st_mode);
}

bool File::IsLink()
{
    if (!exist_)
        return false;
    return S_ISLNK(file_stat_.st_mode);
}

std::string File::GetAbsolutePath()
{
    char absolute_path[1024];
    realpath(path_.c_str(), absolute_path);
    return std::string(absolute_path);
}

std::string File::GetName()
{
    std::string absolute_path = GetAbsolutePath();
    return absolute_path.erase(0, absolute_path.find_last_of("/") + 1);
}

std::string File::GetExt()
{
    std::string name = GetName();
    size_t p = name.find_last_of('.');
    if(p == -1 || p == name.length() - 1)
        return "";
    else{
        std::string ext = name.substr(p + 1);
        //StringUtils::toLower(ext);
        return ext;
    }
}

std::string File::GetMimeType()
{
    return "";
}

off_t File::GetSize()
{
    if(!exist_)
        return 0;
    return file_stat_.st_size;
}

std::string File::ReadAsText()
{
    char buffer[4096];
    std::ifstream in(path_, std::ios_base::in);
    if(!in) 
        return "";
    std::string content = "";
    std::string line;
    while(!in.eof())
    {
        getline(in, line);
        content += line;
        content += '\n';
    }
    content.erase(content.end()-1);
    return content;
}

std::vector<File> File::ListDir()
{

}