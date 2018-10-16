#include "file.h"
#include "util.h"
#include "iobuffer.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <fstream>

// 静态成员变量初始化
std::unordered_map<std::string, std::string> File::mime_ = {
    {"default", "text/plain"},
    {"html", "text/html"},
    {"htm", "text/html"},
    {"txt", "text/plain"},
    {"csv", "text/csv"},
    {"jpeg", "image/jpeg"},
    {"jpg", "image/jpeg"},
    {"png", "image/png"},
    {"gif", "image/gif"},
    {"bmp", "image/bmp"},
    {"ico", "image/x_icon"},
    {"mp3", "audio/mp3"},
    {"avi", "video/x-msvideo"},
    {"json", "application/json"},
    {"doc", "application/msword"},
    {"doc", "application/msword"},
    {"ppt", "application/vnd.ms-powerpoint"},
    {"pdf", "application/pdf"},
    {"gz", "application/x-gzip"},
    {"tar", "application/x-tar"}
};


File::File(std::string path) :
    path_(path)
{
    // 将路径后多余的斜杠去掉
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

Timestamp File::GetCreateTime()
{
    if (!exist_)
        return system_clock::now();
    return util::TimespecToTimestamp(file_stat_.st_ctim);
}

Timestamp File::GetModifyTime()
{
    if (!exist_)
        return system_clock::now();
    return util::TimespecToTimestamp(file_stat_.st_mtim);
}

std::string File::GetName()
{
    std::string absolute_path = path_;
    // 取路径最后一个斜杠后的内容
    return absolute_path.erase(0, absolute_path.find_last_of("/") + 1);
}

std::string File::GetExt()
{
    std::string name = GetName();
    size_t p = name.find_last_of('.');
    // 没有拓展名或者为空直接返回“”
    if(p == -1 || p == name.length() - 1)
        return "";
    else
    {
        std::string ext = name.substr(p + 1);
        util::ToLower(ext);
        return ext;
    }
}

std::string File::GetMimeType()
{
    if (mime_.find(GetExt()) == mime_.end())
    {
        return mime_["default"];
    }
    return mime_[GetExt()];
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
    // 将文件每一行内容读取出来添加到string中
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
    std::string absolute_path = path_;

    DIR *ptr_dir;
    struct dirent *dir_entry;
    ptr_dir = opendir(absolute_path.c_str());

    std::vector<File> files;

    while((dir_entry = readdir(ptr_dir)) != NULL){
        // 忽略"." & ".."
        if(strcmp(dir_entry->d_name, ".") == 0 || strcmp(dir_entry->d_name, "..") == 0 )
            continue;
        // 创建文件或子目录的file类
        files.push_back(File(absolute_path+"/"+dir_entry->d_name));
    }
    return files;
}

void File::ReadToBuffer(IOBuffer& buffer)
{
    // 打开文件描述符
    int src_fd = open(path_.c_str(), O_RDONLY, 0);
    if (src_fd < 0)
    {
        close(src_fd);
        return;
    }

    // 映射文件到内存中
    void *mmap_ret = mmap(NULL, file_stat_.st_size, PROT_READ, MAP_PRIVATE, src_fd, 0);
    close(src_fd);
    if (mmap_ret == (void*)-1)
    {
        munmap(mmap_ret, file_stat_.st_size);
        return;
    }

    // 读取文件
    char *src_addr = static_cast<char*>(mmap_ret);
    buffer.Append(src_addr, file_stat_.st_size);

    // 取消映射
    munmap(mmap_ret, file_stat_.st_size);
    return;
}